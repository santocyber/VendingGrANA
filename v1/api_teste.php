<?php
 
$request_method=$_SERVER["REQUEST_METHOD"];

switch($request_method) {

    case 'GET':
        get_teste();
        break;

    default:
        // Invalid Request Method
        header("HTTP/1.0 405 Method Not Allowed");
        break;
}

function get_teste() {
    $response = "00020101021226940014BR.GOV.BCB.PIX2572pix-qr.mercadopago.com/instore/o/v2/".
    "73055cb8-ceb7-4c9a-8328-298b0630c6c85204000053039865802BR5904Luan6009SAO PAULO62070503***63042300";
    $size = strlen($response);

    header("Content-Type: text/plain");
    header("Content-length: $size");
    echo $response;
}

?>