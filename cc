<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Photo Capture</title>
    <style>
        body {
            text-align: center;
            font-family: Arial, sans-serif;
            background: #f4f4f4;
            display: flex;
            flex-direction: column;
            align-items: center;
            margin: 0;
            padding: 0;
        }

        .container {
            background: white;
            border-radius: 12px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
            padding: 20px;
            margin-top: 20px;
            width: 90%;
            max-width: 500px;
        }

        h2 {
            color: #333;
        }

        video {
            width: 100%;
            border-radius: 10px;
            margin-top: 10px;
            transform: scaleX(-1); /* Mirror live camera preview */
        }

        canvas, img {
            width: 100%;
            border-radius: 10px;
            margin-top: 10px;
        }

        button {
            background: #6d4c41;
            color: white;
            border: none;
            padding: 12px 16px;
            font-size: 16px;
            border-radius: 8px;
            cursor: pointer;
            margin-top: 10px;
            transition: 0.3s;
        }

        button:hover {
            background: #5d4037;
        }

        #confirmationBox {
            display: none;
            position: fixed;
            bottom: 20px;
            left: 50%;
            transform: translateX(-50%);
            background: white;
            padding: 15px;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
            text-align: center;
            width: 90%;
            max-width: 400px;
        }

        .modal button {
            margin: 5px;
        }

        #locationText {
            color: #6d4c41;
            font-weight: bold;
            font-size: 18px;
            margin-top: 15px;
        }
    </style>
</head>
<body>

    <div class="container">
        <h2>Take a Photo with Date, Time, and Location</h2>
        <video id="video" autoplay></video>
        <canvas id="canvas" style="display:none;"></canvas>
        <img id="previewImage" alt="Photo Preview" style="display:none;">
        <p id="locationText">Location: Fetching...</p>
        <button id="capture">Capture Photo</button>
    </div>

    <div id="confirmationBox">
        <p>Do you want to keep this photo?</p>
        <button id="confirm">Confirm</button>
        <button id="retake">Retake</button>
    </div>

    <button id="download" style="display:none;">Download</button>

    <script>
        const video = document.getElementById("video");
        const canvas = document.getElementById("canvas");
        const ctx = canvas.getContext("2d");
        const previewImage = document.getElementById("previewImage");
        const captureBtn = document.getElementById("capture");
        const downloadBtn = document.getElementById("download");
        const locationText = document.getElementById("locationText");
        const confirmationBox = document.getElementById("confirmationBox");
        const confirmBtn = document.getElementById("confirm");
        const retakeBtn = document.getElementById("retake");

        let locationData = "Location not found";
        let attempts = 0;
        const maxAttempts = 3;
        let lastPhotoData = null;

        // Start Camera (Mirrored Preview)
        navigator.mediaDevices.getUserMedia({ video: true })
            .then(stream => { video.srcObject = stream; })
            .catch(err => console.error("Camera access denied:", err));

        // Get Location
        function getLocation() {
            if (navigator.geolocation) {
                navigator.geolocation.getCurrentPosition(
                    async (position) => {
                        const { latitude, longitude } = position.coords;
                        try {
                            const response = await fetch(`https://nominatim.openstreetmap.org/reverse?format=json&lat=${latitude}&lon=${longitude}`);
                            const data = await response.json();
                            locationData = data.display_name || "Unknown Location";
                            locationText.textContent = "Location: " + locationData;
                        } catch (error) {
                            locationText.textContent = "Location fetch failed";
                        }
                    },
                    () => { locationText.textContent = "Location access denied"; }
                );
            }
        }

        getLocation();

        // Capture Photo
        captureBtn.addEventListener("click", () => {
            attempts++;
            takePhoto();

            if (attempts < maxAttempts) {
                confirmationBox.style.display = "block";
            } else {
                confirmPhoto();
            }
        });

        function takePhoto() {
            canvas.width = video.videoWidth;
            canvas.height = video.videoHeight;

            // Flip the captured photo (Mirror horizontally)
            ctx.save();
            ctx.translate(canvas.width, 0);
            ctx.scale(-1, 1);
            ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
            ctx.restore();

            // Date & Time (without seconds)
            const dateTime = new Date().toLocaleString('en-US', {
                year: 'numeric',
                month: '2-digit',
                day: '2-digit',
                hour: '2-digit',
                minute: '2-digit',
                hour12: true
            });

            // Add Date, Time, and Location at the Bottom
            ctx.fillStyle = "#6d4c41"; // Light brown text
            ctx.font = "24px Arial";
            ctx.fillText(dateTime, 20, canvas.height - 70);
            ctx.fillText(locationData, 20, canvas.height - 40);

            // Save the latest photo data
            lastPhotoData = canvas.toDataURL("image/png");

            // Show preview image instead of canvas
            previewImage.src = lastPhotoData;
            previewImage.style.display = "block";
            video.style.display = "none";
        }

        // Confirm Photo
        confirmBtn.addEventListener("click", () => {
            confirmPhoto();
        });

        function confirmPhoto() {
            confirmationBox.style.display = "none";
            downloadBtn.style.display = "block";
            alert("Photo confirmed!");
        }

        // Retake Photo
        retakeBtn.addEventListener("click", () => {
            if (attempts < maxAttempts) {
                confirmationBox.style.display = "none";
                previewImage.style.display = "none";
                video.style.display = "block";
                alert(`Retaking photo... Attempts left: ${maxAttempts - attempts}`);
            }
        });

        // Download Photo
        downloadBtn.addEventListener("click", () => {
            const link = document.createElement("a");
            link.href = lastPhotoData;
            link.download = "photo.png";
            link.click();
        });
    </script>

</body>
</html>
