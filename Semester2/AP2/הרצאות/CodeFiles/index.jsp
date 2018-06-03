<%@ page import="java.util.Date"%>
<%@ page language="java" contentType="text/html; charset=windows-1255"
    pageEncoding="windows-1255"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1255">
<title>My Calculator</title>
</head>
<body>
The server time now is  
<%
	out.println(new Date());
%>
<br>
<form action="MyCalculator" method="POST" target="result">
	<input type="text" name="problem"><br>
	<input type="submit" value="שלח">
</form>
<br>
<iframe name="result">
</iframe>
</body>
</html>