<%@ page import="authenticationPackage.authenticationService.*"%>
<%@ page import="COREMedDemo.COREMedDemoService.*"%>
<%@ page import="dBInfo.dbConnectInfo"%>
<%@ page import="java.sql.*"%>
<%@ page import="java.io.*"%>
<%@ page import="oracle.jdbc.*"%>
<%@ page errorPage="ExceptionHandler.jsp" %>


<%
  String RFIDNum = request.getParameter("RFIDNum");
  
  String lookupType = request.getParameter("lookupType");
  
  // ignore blank requests
    if (RFIDNum == null)
  {
    response.sendError(404);
    System.out.println("Null value.");
    return;
  }
  // sanitize to help prevent sql injection
  RFIDNum = RFIDNum.replaceAll("\\s+", "");
  // if they can't be real tags anyway
  if (RFIDNum.length() > 16)
  {
    response.sendError(404);
    System.out.println("Value too long.");
    return;
  }
  try
  { 
    // display the image
    response.setContentType("image/png");
    OutputStream o = response.getOutputStream();
     
     // get the image from the database
    authenticationPackage.authenticationService as = new authenticationPackage.authenticationService();
    authenticationPackage.itemInfo info = as.getItem(RFIDNum, "FFFFFFFA", 20F, 20F, false);
       
    if (info.getImage() != null)
    {
        o.write(info.getImage());
    }
    else
    {
      COREMedDemo.COREMedDemoService ds = new COREMedDemo.COREMedDemoService();
      COREMedDemo.drugInfo drugInfo = ds.getDrugInfo(RFIDNum,"FFFFFFFA");
      if (drugInfo.isExists())
      {        
        if (drugInfo.getPicture() != null)
          o.write(drugInfo.getPicture());
      }
      else
      {
        response.sendError(404);
        return;
      }
    }      
       
     o.flush();
     o.close();
  }
  catch (Exception e)
  {
    e.printStackTrace();
    throw e;
  }    
%>