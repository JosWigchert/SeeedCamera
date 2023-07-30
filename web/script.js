document.addEventListener('DOMContentLoaded', function() {
    const darkModeToggle = document.getElementById('darkModeToggle');
    const body = document.body;
    darkModeToggle.addEventListener('change', function() {
      body.classList.toggle('dark-mode', darkModeToggle.checked);
      handleToggle('Dark Mode', darkModeToggle.checked);
    });
  });
  
  function handleButtonClick(buttonName) {
    // Implement your desired action when a button is clicked
    alert("Button '" + buttonName + "' clicked!");
  }
  
  document.getElementById('textInput').addEventListener('change', function() {
    const value = this.value;
    handleTextInput(value);
  });
  
  function handleTextInput(value) {
    // Implement your desired action when the text input value changes
    console.log("Text Input Value: " + value);
  }
  
  function handleToggle(label, state) {
    // Implement your desired action when the toggle state changes
    console.log("Toggle '" + label + "' changed: " + state);
  }
  