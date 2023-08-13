//--------------- Handle Callback -------------------

function handleTextInput(event) {
    // Check if the pressed key is the "Enter" key (key code 13)
    if (event.keyCode === 13) {
        // Perform your action here, for example, call a function
        callback(event.target.id, event.target.value);
    }
}

function handleButton(event) {
    callback(event.target.id, null)
}

function handleToggle(event) {
    callback(event.target.id, event.target.checked)
}

function callback(identifier, value) {
    var data = {
        identifier: identifier,
        value: value,
    };

    fetch('api/callback', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response;
        })
        .then(data => {
            // Handle the response data here
            console.log(data);
        })
        .catch(error => {
            // Handle errors here
            console.error('Error:', error);
        });
}

//--------------- Update Values -------------------

function updateValues() {
    fetch('api/values')
        .then(response => response.json())
        .then(data => {
            // Loop over all keys in the JSON response
            for (const key in data) {
                if (data.hasOwnProperty(key)) {

                    // Find the corresponding HTML element using the key as the element ID
                    const element = document.getElementById(key);
                    if (element) {
                        // Update the HTML element's text content with the sanitized value
                        element.textContent = data[key];
                    }
                }
            }
        })
        .catch(error => {
            console.error('Error fetching sensor data:', error);
        });
}

// Call updateSensorValues initially to populate the sensor values
updateValues();

// Call updateSensorValues periodically (e.g., every 5 seconds)
setInterval(updateValues, 500); // Adjust the interval as needed

//--------------- Get Push Values -------------------

const socket = new WebSocket('ws://' + window.location.hostname + ':80/');

socket.onmessage = function (event) {
    console.log(event);
};