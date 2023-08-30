<?php
// Connect to database
include("../connection.php");
$db = new dbObj();
$connection =  $db->getConnstring();
 
$request_method=$_SERVER["REQUEST_METHOD"];

switch($request_method) {
    case 'GET':
        if(!empty($_GET["id"])) {
            $id = intval($_GET["id"]);
            if(!empty($_GET["status"])) {
                get_order_status($id);
            } else {
                get_order($id);
            }
        } else {
            get_orders();
        }
        break;
    default:
        header("HTTP/1.0 405 Method Not Allowed");
        break;
}

function get_order($id) {
    global $connection;
    $query = "SELECT * FROM order_data WHERE external_id = ?;";
    $stmt = mysqli_prepare($connection, $query);
    mysqli_stmt_bind_param($stmt, 'i', $id);
    mysqli_stmt_execute($stmt);
    $result = mysqli_stmt_get_result($stmt);
    
    $response = array();
    while ($row = mysqli_fetch_object($result)) {
        $response[] = $row;
    }
    mysqli_stmt_close($stmt);

    $response_data = json_encode($response[0], JSON_NUMERIC_CHECK);
    $size = strlen($response_data);
    header("Content-Type: application/json");
    header("Content-length: $size");
    echo $response_data;

}

function get_order_status($id) {
    global $connection;
    $query = "SELECT * FROM order_data WHERE external_id = ?;";
    $stmt = mysqli_prepare($connection, $query);
    mysqli_stmt_bind_param($stmt, 'i', $id);
    mysqli_stmt_execute($stmt);
    $result = mysqli_stmt_get_result($stmt);
    
    $response = array();
    while ($row = mysqli_fetch_object($result)) {
        $response[] = $row;
    }
    mysqli_stmt_close($stmt);

    $size = 1;
    header("Content-Type: text/plain");
    header("Content-length: $size");
    echo $response[0]->order_status;

}

function get_orders() {
    global $connection;
    $query = "SELECT * FROM order_data";
    $result = mysqli_query($connection, $query);
    
    $response = array();
    while ($row = mysqli_fetch_object($result)) {
        $response[] = $row;
    }
    mysqli_close($connection);

    $response_data = json_encode($response, JSON_NUMERIC_CHECK);
    $size = strlen($response_data);
    header("Content-Type: application/json");
    header("Content-length: $size");
    echo $response_data;

}

?>