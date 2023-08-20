//--------------- Handle Callback -------------------

function handleTextInput(event) {
    // Check if the pressed key is the "Enter" key (key code 13)
    validateInput(event)
    if (event.keyCode === 13 && !event.target.classList.contains("invalid-input")) {
        // Perform your action here, for example, call a function
        callback(event.target.id, event.target.value);
        console.log("Input");
    }
}

function handleButton(event) {
    callback(event.target.id, null)
    console.log("Button");
}

function handleToggle(event) {
    callback(event.target.id, event.target.checked)
    console.log("Toggle");
}

function handleComboBox(event) {
    callback(event.target.id, event.target.value)
    console.log("ComboBox");
}

const inputs = document.querySelectorAll('input');
inputs.forEach(input => {
    input.addEventListener('keydown', handleTextInput);
});

const toggles = document.querySelectorAll('input[type="checkbox"]');
toggles.forEach(toggle => {
    toggle.addEventListener('change', handleToggle);
});

const buttons = document.querySelectorAll('button');
buttons.forEach(button => {
    button.addEventListener('click', handleButton);
});

const comboboxes = document.querySelectorAll('select');
comboboxes.forEach(combobox => {
    combobox.addEventListener('change', handleComboBox);
});

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
setInterval(updateValues, 1500); // Adjust the interval as needed

//--------------- Get Push Values -------------------

// const socket = new WebSocket('ws://' + window.location.hostname + ':80/');

// socket.onmessage = function (event) {
//     console.log(event);
// };

//--------------- Validate Inputs -------------------
inputs.forEach(input => {
    input.addEventListener('input', validateInput);
    input.addEventListener('keydown', restrictInput);
});

function validateInput(event) {
    const input = event.target;
    input.classList.remove("invalid-input");

    switch (input.type) {
        case 'url':
        case 'text':
            if (!input.value.trim()) {
                input.classList.add("invalid-input");
            }
            break;
        case 'number':
            if (isNaN(input.value)) {
                input.classList.add("invalid-input");
            }
            break;
        case 'email':
            const emailPattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
            if (!emailPattern.test(input.value)) {
                input.classList.add("invalid-input");
            }
            break;
        case 'date':
            const isValidDate = !isNaN(Date.parse(input.value));
            if (!isValidDate) {
                input.classList.add("invalid-input");
            }
            break;
        case 'tel':
            const telPattern = /^[\d\s()+-]*$/;
            if (!telPattern.test(input.value)) {
                input.classList.add("invalid-input");
            }
            break;
    }
}
function restrictInput(event) {
    const input = event.target;

    if (input.type === 'number') {
        // Allow only numeric input and Backspace for number fields
        const isNumericKey = /[0-9]/.test(event.key);
        const isBackspace = event.key === 'Backspace';
        if (!isNumericKey && !isBackspace) {
            event.preventDefault();
        }
    }
}

