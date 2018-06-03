<%@page import="model.ResultBean"%>
<%@ page language="java" contentType="text/html; charset=windows-1255"
    pageEncoding="windows-1255"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=windows-1255">
<title>Insert title here</title>
</head>
<body>
<h1>
<% String problem=request.getParameter("problem");
	double result=((ResultBean)request.getAttribute("resultBean")).getResult();
	out.println(problem+" = "+result);
%>
</h1>
</body>
</html>