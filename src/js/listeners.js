const marker = document.querySelector("#indicator");
const items = document.querySelectorAll(".side-tab");

items.forEach(link => link.addEventListener("click",
    e => indicator(e.target)));

function indicator(item) {
    marker.style.top = item.offsetTop + "px";
    marker.style.height = "30px";
}