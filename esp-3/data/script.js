var temp1 = "default";
var temp2 = "default";
var hum1 = "default";
var hum2 = "default";
var soil1 = "default";
var soil2 = "default";
var response = new Response();

var debug_mode = false;

function DebugMode() {
    debug_mode = !debug_mode;
    if (debug_mode) {
        document.getElementById("test_btn").style.display = "block";
        document.getElementById("debug_btn").innerHTML = "Выключение debug режима (этого ты вообще не должен видеть!)";
    }
    else {
        document.getElementById("test_btn").style.display = "";
        document.getElementById("debug_btn").innerHTML = "Включение debug режима";
    }
}

function SetAngle(angle) {
    fetch("/angle"+angle)
}

function UpdateTest() {
    if (temp1=="default") temp1 = 0.0;
    temp1+=Math.random()*5-2.5;

    if (temp2=="default") temp2 = 0.0;
    temp2+=Math.random()*5-2.5;

    if (hum1=="default") hum1 = 0.0;
    hum1+=Math.random()*5-2.5;

    if (hum2=="default") hum2 = 0.0;
    hum2+=Math.random()*5-2.5;
    
    if (soil1=="default") soil1 = 0.0;
    soil1+=Math.random()*5-2.5;
    
    if (soil2=="default") soil2 = 0.0;
    soil2+=Math.random()*5-2.5;
}

function AutoUpdate() {
    //temp 1
    if (!debug_mode) {
        response = fetch("/temp1");
        temp1 = response.text();
    }
    document.getElementById("temp1").innerHTML = temp1.toFixed(2);

    //temp 2
    if (!debug_mode) {
        response = fetch("/temp2");
        temp2 = response.text();
    }
    document.getElementById("temp2").innerHTML = temp2.toFixed(2);

    //hum1
    if (!debug_mode) {
        response = fetch("/hum1");
        hum1 = response.text();
    }
    document.getElementById("hum1").innerHTML = hum1.toFixed(2);

    //hum2
    if (!debug_mode) {
        response = fetch("/hum2");
        hum2 = response.text();
    }
    document.getElementById("hum2").innerHTML = hum2.toFixed(2);
    
    //soil1
    if (!debug_mode) {
        response = fetch("/soil1");
        soil1 = response.text();
    }
    document.getElementById("soil1").innerHTML = soil1.toFixed(2);
    
    //soil2
    if (!debug_mode) {
        response = fetch("/soil2");
        soil2 = response.text();
    }
    document.getElementById("soil2").innerHTML = soil2.toFixed(2);
}

setInterval(AutoUpdate, 500);
