<html>
	<head>
	<title>Telemetria GrANANET</title>
</head>
<body>

	<h1>Pagina mostrando dados do banco de dados da telemetria GrANANET</h1>

<?php
// Instancia o objeto PDO
$pdo = new PDO('mysql:host=santocyber.helioho.st;dbname=santocyber_telemetria', 'santocyber_telemetria', 'telemetria');
// define para que o PDO lance exceções caso ocorra erros
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
?>

<?php
// executa a instrução SQL
$consulta = $pdo->query("SELECT identificador, nome, ip, granaentrada, objsaida, mac, timelocal, last_update FROM telemetria;");
?>


<?php
while ($linha = $consulta->fetch(PDO::FETCH_ASSOC)) {
    // aqui eu mostro os valores de minha consulta
    echo "Registro N: {$linha['identificador']} - Nome: {$linha['nome']} - Valor entrada: {$linha['granaentrada']} - Quantidade Saida: {$linha['objsaida']} - Endereço MAC: {$linha['mac']} - IP local: {$linha['ip']} - Hora local: {$linha['timelocal']} - Ultima Atualizacao: {$linha['last_update']}<br />";
}
?>




