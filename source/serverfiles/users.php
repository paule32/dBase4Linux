<?php
//echo base64_encode("user1621|geheim|2016-07-15|my@dom.tld|2016-07-13");
if (!isset($_GET['exp'])||!empty($_GET['exp'])) { $exp=htmlspecialchars($_GET['exp']); }
else $exp="";$req=$_GET['checker'];if(strlen($exp)>1){echo htmlspecialchars($exp);return;}
if(strlen($req)<1){echo htmlspecialchars("wrong data.");return;}$str=htmlspecialchars(base64_decode($req));
$str=explode('|',$str);$user=htmlspecialchars($str[0]);$pass=htmlspecialchars($str[1]);
$demo=htmlspecialchars($str[2]);$mail=htmlspecialchars($str[3]);$last=$str[4];$flag=1;
try{$dbh=new PDO(htmlspecialchars('sqlite:/var/www/dbase.tools/data/hd_data.db'));
}catch(PDOException $e){error_log($e->getMessage());echo base64_encode("error: database not found/can not install.");
return;}$res=$dbh->query("CREATE TABLE IF NOT EXISTS hd_users ("
."id INTEGER PRIMARY KEY AUTOINCREMENT,"."user VARCHAR(30),"."pass VARCHAR(30),"
."demo VARCHAR(10),"."mail VARCHAR(60),"."last VARCHAR(10),"."flag CHAR(1))");
$res=$dbh->prepare("SELECT user,pass,last FROM hd_users WHERE user='$user' AND pass='$pass';");
$res->execute([$user,$pass]);$arr=$res->fetchAll();if(!$arr){$curdat=date("Y-m-d");$demo=$curdat;
$res=$dbh->prepare("INSERT INTO hd_users (user,pass,demo,mail,last,flag)VALUES(:user,:pass,:demo,:mail,:last,:flag);");
$res->bindParam(":user",$user,PDO::PARAM_STR,30);$res->bindParam(":pass",$pass,PDO::PARAM_STR,30);
$res->bindParam(":demo",$demo,PDO::PARAM_STR,10);$res->bindParam(":mail",$mail,PDO::PARAM_STR,60);
$res->bindParam(":last",$last,PDO::PARAM_STR,10);$res->bindParam(":flag",$flag,PDO::PARAM_INT);$res->execute();}try{
$res=$dbh->prepare("SELECT user,pass,demo,last FROM hd_users WHERE user=:user AND pass=:pass;");
$res->bindParam(":user",$user,PDO::PARAM_STR,30);$res->bindParam(":pass",$pass,PDO::PARAM_STR,30);
$res->execute();$arr=$res->fetchAll();var_dump($arr);}catch(PDOException $e){echo "error";return;}
$date=$arr[0]['demo'];$last=$arr[0]['last'];$expire=date("Y-m-d",strtotime($date));
$lstdat=date("Y-m-d",strtotime($last));$curdat=date("Y-m-d");if((($expire<=$curdat)||($lstdat<=$expire))
&&(!strcmp($user,$arr[0]['user']))&&(!strcmp($pass,$arr[0]['pass']))){
header("Location: users.php?checker=1&exp=".base64_encode("Error: Licence-Expire!"));return;}
echo base64_encode("ok");?>
