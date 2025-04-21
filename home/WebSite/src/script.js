document.getElementById('form').addEventListener('submit', function(e) {
    e.preventDefault();

    const endpoint = "http://localhost:8080/upload"
    const userFile = document.getElementById('user-file').files[0];
    const formData = new FormData();
    
    formData.append('user-file', userFile, 'user_file.tar');
    fetch(endpoint, {
        method: 'POST',
        body: formData,
    })
    .then(res => res.json())
    .then(data => console.log(data))
    .catch(err => console.log(err));
});

function displayTheDataOnThePage() {
    const container = document.getElementById('content-box-3');
    const div = document.createElement('div');
    div.classList.add('content-div');
    div.innerHTML = `
        <p id='content-title'> Test Title </p>
        <p id='file-content'>
            Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
        </p>
    `;

    if (container)
        container.appendChild(div);
    else
        alert("Failed to display file contents");
}