<?php
// Connect to database
include("../connection.php");
$db = new dbObj();
$connection =  $db->getConnstring();

$config = parse_ini_file("../gateway_config.ini", true);
$pix_webhook_url = $config['pix']['wbhook_url'];
$pix_qrcode_duration = $config['pix']['qrcode_duration'];
$pix_token = $config['pix']['token'];
$pix_user_id = $config['pix']['user_id'];
$pix_external_pos_id = $config['pix']['external_pos_id'];

$request_method=$_SERVER["REQUEST_METHOD"];

switch($request_method) {

    case 'GET':
        get_qrcode_data();
        break;

    default:
        // Invalid Request Method
        header("HTTP/1.0 405 Method Not Allowed");
        break;
}

function get_expiration() {
    global $pix_qrcode_duration;
    $current_time = new DateTime('NOW');
    $current_time->setTimezone(new DateTimeZone('America/Sao_Paulo'));
    $current_time->add(new DateInterval('PT' . $pix_qrcode_duration . 'M'));
    $stamp =  $current_time->format('Y-m-d\TH:i:s.vP');
    return $stamp;
}

function create_order($external_id, $value = 0.25) {
    global $pix_token;
    global $pix_webhook_url;
    global $pix_user_id;
    global $pix_external_pos_id;
    $order_data = array(
        "external_reference" => $external_id,
        "title" => "Compra pix teste",
        "description" => "Compra pix teste",
        "notification_url" => $pix_webhook_url,
        "expiration_date" => get_expiration(),
        "total_amount" => $value,
        "items" => array(
            array(
                "title" => "Item de teste",
                "description" => "Item de teste",
                "unit_price" => $value,
                "quantity" => 1,
                "unit_measure" => "unit",
                "total_amount" => $value
            )
        )
    );

    $url = "https://api.mercadopago.com/instore/orders/qr/seller/collectors/" . $pix_user_id . 
    "/pos/" . $pix_external_pos_id . "/qrs";

    $curl = curl_init($url);
    curl_setopt($curl, CURLOPT_URL, $url);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($curl, CURLOPT_POST, 1);
    $headers = array(
        "Authorization: Bearer " . $pix_token,
        "Content-Type: application/json"
    );
    curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($order_data));
    $response = curl_exec($curl);
    curl_close($curl);
    
    $response_object =  json_decode($response);

    $qr_data = $response_object->{"qr_data"};
    $size = strlen($qr_data);

    header("Content-Type: text/plain");
    header("Content-length: $size");
    echo $qr_data;

}

function order_exists($external_id) {
    global $connection;
    $query = "SELECT * FROM order_data WHERE external_id = ?;";
    $stmt = mysqli_prepare($connection, $query);
    mysqli_stmt_bind_param($stmt, 'i', $external_id);
    mysqli_stmt_execute($stmt);
    $result = mysqli_stmt_get_result($stmt);

    $response = array();
    while ($row = mysqli_fetch_object($result)) {
        $response[] = $row;
    }

    mysqli_stmt_close($stmt);

    if (count($response) > 0) {
        return true;
    } else {
        return false;
    }
}

function get_qrcode_data() {
    
    if(!empty($_GET["id"])) {
        $id = $_GET["id"];
        if(!order_exists($id)) {
            if(!empty($_GET["value"])) {
                $value = floatval($_GET["value"]);
                create_order($id, $value);
            } else {
                create_order($id);
            }
        } else {
            http_response_code(409);
            header("Content-Type: text/plain");
            echo "A Ordem ".$id." já existe";
        }
        

    } else {
        http_response_code(400);
        header("Content-Type: text/plain");
        echo "É preciso informar o campo ID";
    }
    

}

?>