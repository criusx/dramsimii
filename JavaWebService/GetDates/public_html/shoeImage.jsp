<%@ page import="ShoePackage.shoePair"%>
<%@ page import="ShoePackage.shoeService"%>
<%@ page import="dBInfo.dbConnectInfo"%>
<%@ page import="java.sql.*"%>
<%@ page import="java.io.*"%>
<%@ page import="oracle.jdbc.*"%>
<%@ page errorPage="ExceptionHandler.jsp" %>


<%
  String RFIDNumA = request.getParameter("RFIDNumA");
  String RFIDNumB = request.getParameter("RFIDNumB");
  
  // ignore blank requests
  if (RFIDNumA == null || RFIDNumB == null)
  {
    response.sendError(404);
    System.out.println("Null value.");
    return;
  }
  
  try
  {  
     // get the image from the database
    shoeService as = new shoeService();
    shoePair info = as.getItem(RFIDNumA, RFIDNumB, "FFFFFFFA", 20F, 20F, false);
       
    if (info.getImage() != null)
    {
      // display the image
      OutputStream o = response.getOutputStream();
      response.setContentType("image/png");
      o.write(info.getImage());
      o.flush();
      o.close();
    }
    else
    {
      response.sendError(404);
      return;
    }
  }
  catch (Exception e)
  {
    e.printStackTrace();
    throw e;
  }    
%>