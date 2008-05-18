<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<%@ page contentType="text/html;charset=windows-1252"%>
<%@ page isErrorPage="true" import="java.io.*" %>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=windows-1252"/>
    <title>ExceptionHandler</title>
  </head>
  <body>
  <%-- Exception Handler --%>
  <%= exception.toString() %><br />
  <%
  out.println("<!--");
StringWriter sw = new StringWriter();
PrintWriter pw = new PrintWriter(sw);

exception.printStackTrace(pw);
out.print(sw);
sw.close();
pw.close();
out.println("-->");
%>

  </body>
</html>