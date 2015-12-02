<?php
// www.internetdelascosas.cl
//
// Script para recolectar datos enviados por arduinos conectados a la red
//
// contacto@internetdelascosas.cl
//

// Parametros para encender leds
$umbralTemperatura = 22;
$umbralHumedad = 80;
$umbralLuminosidad = 500;

// Parametros de base de datos
$mysql_servidor = "localhost";
$mysql_base = "iot";
$mysql_usuario = "iot";
$mysql_clave = "contraseña_super_segura_que_no_es_hola123";

$id = htmlspecialchars($_GET["id"],ENT_QUOTES);
$temperatura = htmlspecialchars($_GET["temperatura"],ENT_QUOTES);
$humedad = htmlspecialchars($_GET["humedad"],ENT_QUOTES);
$luminosidad = htmlspecialchars($_GET["luminosidad"],ENT_QUOTES);

// Valida que esten presente todos los parametros
if (($id!="") and ($temperatura!="") and ($humedad!="") and ($luminosidad!="")) {
    mysql_connect($mysql_servidor,$mysql_usuario,$mysql_clave) or 
       die("Imposible conectarse al servidor.");
    mysql_select_db($mysql_base) or die("Imposible abrir Base de datos");

    $sql = "insert into variable (fecha, id, nombre, valor) 
            values (NOW(),'$id','temperatura','$temperatura')";
    mysql_query($sql);

    $sql = "insert into variable (fecha, id, nombre, valor) 
            values (NOW(),'$id','humedad','$humedad')";
    mysql_query($sql);

    $sql = "insert into variable (fecha, id, nombre, valor) 
            values (NOW(),'$id','luminosidad','$luminosidad')";
    mysql_query($sql);

    // Genera respuesta en XML para que el Arduino lo procese
    if ($temperatura < $umbralTemperatura)
        echo "<led_1>1</led_1>";
    else
        echo "<led_1>0</led_1>";

    if ($humedad > $umbralHumedad)
        echo "<led_2>1</led_2>";
    else
        echo "<led_2>0</led_2>";

    if ($luminosidad < $umbralLuminosidad)
        echo "<led_3>1</led_3>";
    else
        echo "<led_3>0</led_3>";
    echo "\n";
}
?>