<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    // Certifique-se de que o método de solicitação seja POST

    try {
        $pdo = new PDO('mysql:host=santocyber.helioho.st;dbname=santocyber_telemetria', 'santocyber_telemetria', 'telemetria');
        $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

        foreach ($_POST['action'] as $mac => $action) {
            // Verifique se a ação é uma das ações permitidas para segurança adicional
            $allowedActions = ['ligado', 'desligado', 'reinicia', 'credito'];
            if (!in_array($action, $allowedActions)) {
                echo "Ação inválida: " . htmlspecialchars($action);
                continue; // Pule ação inválida
            }

            // Execute aqui a lógica para cada ação, usando $mac e $action
            // Por exemplo, você pode executar consultas SQL ou chamar funções para processar as ações

            // Aqui você pode tratar cada ação individualmente, por exemplo:
            if ($action === 'ligado') {
                // Lógica para ligar a telemetria
            } elseif ($action === 'desligado') {
                // Lógica para desligar a telemetria
            } elseif ($action === 'reinicia') {
                // Lógica para reiniciar a telemetria
            } elseif ($action === 'credito') {
                // Lógica para enviar crédito para o ESP32
            }

            // Atualize a tabela no banco de dados com a ação realizada
            $stmt = $pdo->prepare("UPDATE telemetria SET acoes = :acao WHERE mac = :mac");
            $stmt->bindParam(':acao', $action);
            $stmt->bindParam(':mac', $mac);
            $stmt->execute();
        }

        // Redireciona de volta para a página telemetria.php após o processamento
        header("Location: telemetria.php");
        exit(); // Certifique-se de sair após o redirecionamento
    } catch (PDOException $e) {
        echo "Erro: " . $e->getMessage();
    }
} else {
    echo "Acesso não autorizado.";
}
?>
