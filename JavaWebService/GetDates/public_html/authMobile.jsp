<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.3//EN" "http://www.wapforum.org/DTD/wml13.dtd">

<%@ page import="authenticationPackage.authenticationService.*"%>
<%@ page import="COREMedDemo.COREMedDemoService.*"%>
<%@ page import="dBInfo.dbConnectInfo"%>
<%@ page import="java.sql.*"%>
<%@ page import="java.io.*"%>
<%@ page import="oracle.jdbc.*"%>
<%@ page errorPage="ExceptionHandler.jsp"%>

<wml>
    <card id="card1" title="WML Form">
        <% response.setContentType("text/vnd.wap.wml"); %>
        <p>
            Entered Code:<br /> 
            <%= request.getParameter("code") %><br/>
            Product Description: <br />
            <%
            String RFIDNum = request.getParameter("code");
            
            String lookupType = null;
            
            boolean validCode = true;
            
            // ignore blank requests
            if (RFIDNum == null)
            {
            System.out.println("Null value.");
            validCode = false;
            }
            // sanitize to help prevent sql injection
            RFIDNum = RFIDNum.replaceAll("\\s+", "");
            // if they can't be real tags anyway
            if (RFIDNum.length() > 16)
            {
            System.out.println("Value too long.");
            validCode = false;
            }
            try
            { 
              // only accept ids in hex
              for (int i = 0; i < RFIDNum.length(); i++)
              {
                if (!("0123456789abcdefABCDEF".indexOf(RFIDNum.charAt(i)) >= 0))
                {
                  System.out.println("Non-hex character encountered");
                  validCode = false;
                }
              }
              
              String description = null;
              boolean authenticated = false;
              
              if (validCode)
              { 
                authenticationPackage.authenticationService as = new authenticationPackage.authenticationService();
                authenticationPackage.itemInfo info = as.getItem(RFIDNum, "FFFFFFFA", 20F, 20F);
                
                if (info.isExists())
                {
                  description = info.getDescription().replaceAll("\n","<br />");
                  authenticated = true;
                }              
                else
                {
                  COREMedDemo.COREMedDemoService ds = new COREMedDemo.COREMedDemoService();
                  COREMedDemo.drugInfo drugInfo = ds.getDrugInfo(RFIDNum,"FFFFFFFA");
                  
                  if (drugInfo.isExists())
                  {
                    description = drugInfo.getDescription().replaceAll("\n","<br />");
                    authenticated = drugInfo.isAuthenticated();
                  }
                }
              }
              
              if (description != null)
              {
                out.println(description);
                out.println("<br/>Image:<br/><img alt=\"Missing Image\" src=\"http://id2.gentag.com/GentagDemo/tagImage.jsp?RFIDNum=" + RFIDNum + "\"/>");
              }
              else
                out.println("No description found.");
                
              if (authenticated)
                out.println("<br /><img src=\"images/ok.png\" alt=\"authenticated\" />");
              else
                out.println("<br /><img src=\"images/cancel.png\" alt=\"not authenticated\" />");
            }
            catch (Exception e)
            {
              e.printStackTrace();
              throw e;
            }            
            %>
        </p>
        <p>
          <br /> Please enter another authentication code.<br />
            <input name="authCode" /><br />
            <anchor><go href="authMobile.jsp" method="post">
            <postfield name="code" value="$(authCode)" />
            </go>Submit</anchor>
        </p>
    </card>
</wml>