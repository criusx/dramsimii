<%@ page import="MedReminder.medReminderService.*"%>
<%@ page import="MedReminder.medReminderService"%>
<%@ page import="MedReminder.drugInfo"%>
<%@ page import="java.io.*"%>
<%@ page errorPage="ExceptionHandler.jsp" %>


<%
  String RFIDNum = request.getParameter("ID");
  
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
    
     
     // get the image from the database
    medReminderService rs = new medReminderService();
    drugInfo info = rs.getDrug(RFIDNum, "FFFFFFFB");
       
    if (info.getPicture() != null)
    {
      OutputStream o = response.getOutputStream();
      o.write(info.getPicture());
      o.flush();
      o.close();
    }
    else
    {
      response.sendError(404);            
    }      
  }
  catch (Exception e)
  {
    e.printStackTrace();
    throw e;
  }    
%>