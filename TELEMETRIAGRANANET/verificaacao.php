<?php
// Informações de conexão com o banco de dados
$hostname = 'santocyber.helioho.st';
$username = 'santocyber_telemetria';
$password = 'telemetria';
$database = 'santocyber_telemetria';

try {
    // Conectar ao banco de dados usando PDO
    $pdo = new PDO("mysql:host=$hostname;dbname=$database", $username, $password);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Verificar se 'mac' está definido na URL
    if (isset($_GET['mac'])) {
        $mac = $_GET['mac'];
        
        // Sanitizar o valor do parâmetro 'mac' para evitar SQL injection
        $mac_value = filter_var($mac, FILTER_SANITIZE_STRING);
        
        // Preparar a consulta SQL usando um placeholder
        $sql = "SELECT acoes FROM telemetria WHERE mac = :mac";

        // Preparar a declaração SQL
        $stmt = $pdo->prepare($sql);

        // Bind do valor do parâmetro
        $stmt->bindParam(':mac', $mac_value, PDO::PARAM_STR);

        // Executar a consulta
        $stmt->execute();

        // Obter o resultado da consulta
        $row = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($row) {
            $action = $row['acoes'];

            // Retornar a ação como resposta JSON
            $response = array('acoes' => $action);
            echo json_encode($response);
        } else {
            // A ação não foi encontrada
            echo json_encode(array('error' => 'Ação não encontrada'));
        }
    } else {
        // 'mac' não está definido na URL
        echo json_encode(array('error' => 'Parâmetro "mac" não especificado na URL'));
    }
} catch (PDOException $e) {
    // Tratar erros de conexão com o banco de dados
    echo json_encode(array('error' => 'Erro de conexão com o banco de dados: ' . $e->getMessage()));
}
?>
