let allowDataDisplay = false; // Giá trị mặc định

function setAllowDataDisplay(value) {
    allowDataDisplay = value;
}

function getAllowDataDisplay() {
    return allowDataDisplay;
}

module.exports = { setAllowDataDisplay, getAllowDataDisplay };
