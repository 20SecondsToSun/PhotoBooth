<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<title>API logout</title>
<?php
    $aid = $_GET["aid"];
    $token = $_GET["token"];
    $domain = "familyagency.ru";
    echo "<script src=\"https://login.vk.com/?act=openapi&oauth=1&aid=$aid&location=$domain&do_logout=1&token=$token\"></script>";
?>
</head>
<body>
VK API Logout
</body>
</html>zn