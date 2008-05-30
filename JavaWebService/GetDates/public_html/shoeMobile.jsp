<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.3//EN" "http://www.wapforum.org/DTD/wml13.dtd">

<%@ page import="ShoePackage.shoePair"%>
<%@ page import="ShoePackage.shoeService"%>
<%@ page import="java.io.*"%>
<%@ page errorPage="ExceptionHandler.jsp"%>

<wml>
    <card id="card1" title="WML Form">
        <% response.setContentType("text/vnd.wap.wml"); %>
        <p>
            Entered Codes:<br /> 
            <%= request.getParameter("codeA") %><br />
            <%= request.getParameter("codeB") %><br />
            Product Description: <br />
            <%
            String RFIDNumA = request.getParameter("codeA");
            RFIDNumA = RFIDNumA.trim();
            String RFIDNumB = request.getParameter("codeB");
            RFIDNumB = RFIDNumB.trim();
            
            try
            { 
              String description = null;
              boolean authenticated = false;


              shoeService ss = new shoeService();
              shoePair info = ss.getItem(RFIDNumA, RFIDNumB, "FFFFFFFA", 20F, 20F);

              if (info.isExists())
              {
                description = info.getDescription().replaceAll("\n","<br />");
                authenticated = true;
              }              
              
              if (description != null)
              {
                out.println(description);
                out.println("<br/>Image:<br/><img alt=\"Missing Image\" src=\"http://id2.gentag.com/GentagDemo/shoeImage.jsp?RFIDNumA=" + RFIDNumA + "&amp;RFIDNumB=" + RFIDNumB + "\"/>");
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
          <br /> Please enter the authentication codes.<br />
            <input name="authCodeA" /><br />
            <input name="authCodeB" /><br />
            <anchor><go href="shoeMobile.jsp" method="post">
                    <postfield name="codeA" value="$(authCodeA)" />
                    <postfield name="codeB" value="$(authCodeB)" />
                </go>Submit</anchor>
        </p>
    </card>
</wml>