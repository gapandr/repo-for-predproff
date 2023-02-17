var temp1 = -200.00;
var temp2 = -200.00;
var hum1 = -200.00;
var hum2 = -200.00;
var soil1 = -200.00;
var soil2 = -200.00;
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

async function AutoUpdate() {
    //temp 1
    if (!debug_mode) {
        response = await fetch("/temp1");
        response.text().then(function(result) {temp1 = parseFloat(result)});
    }
    document.getElementById("temp1").innerHTML = temp1.toFixed(2);

    //temp 2
    if (!debug_mode) {
        response = await fetch("/temp2");
        response.text().then(function(result) {temp2 = parseFloat(result)});
    }
    document.getElementById("temp2").innerHTML = temp2.toFixed(2);

    //hum1
    if (!debug_mode) {
        response = await fetch("/hum1");
        response.text().then(function(result) {hum1 = parseFloat(result)});
    }
    document.getElementById("hum1").innerHTML = hum1.toFixed(2);

    //hum2
    if (!debug_mode) {
        response = await fetch("/hum2");
        response.text().then(function(result) {hum2 = parseFloat(result)});
    }
    document.getElementById("hum2").innerHTML = hum2.toFixed(2);
    
    //soil1
    if (!debug_mode) {
        response = await fetch("/soil1");
        response.text().then(function(result) {soil1 = parseFloat(result)});
    }
    document.getElementById("soil1").innerHTML = soil1.toFixed(2);
    
    //soil2
    if (!debug_mode) {
        response = await fetch("/soil2");
        response.text().then(function(result) {soil2 = parseFloat(result)});
    }
    document.getElementById("soil2").innerHTML = soil2.toFixed(2);
}

setInterval(AutoUpdate, 2000);
