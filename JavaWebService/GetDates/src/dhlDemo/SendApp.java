package dhlDemo;

import java.util.*;

import java.io.*;

import javax.mail.*;
import javax.mail.internet.*;

import javax.activation.*;

import com.sun.mail.smtp.*;

public class SendApp
{

  private static final String username = "feedback@gentag.com";
  private static final String password = "gentag2006";
  private static final String hostName = "smtp.gmail.com";
  private static final boolean debug = false;

  public void send(String smtpHost, int smtpPort, String from, String to, 
                   String subject, String body)
    throws AddressException, MessagingException
  {
    // Create a mail session
    java.security.Security.addProvider(new com.sun.net.ssl.internal.ssl.Provider());
    Properties props = new Properties();
    props.put("mail.smtp.user", username);
    props.put("mail.smtp.host", hostName);
    props.put("mail.smtp.port", "465");
    props.put("mail.smtp.starttls.enable", "true");
    props.put("mail.smtp.auth", "true");
    //props.put("mail.smtp.debug", "true");
    props.put("mail.smtp.socketFactory.port", "465");
    props.put("mail.smtp.socketFactory.class", 
              "javax.net.ssl.SSLSocketFactory");
    props.put("mail.smtp.socketFactory.fallback", "false");

    SecurityManager security = System.getSecurityManager();

    try
    {
      Authenticator auth = new SMTPAuthenticator();
      Session session = Session.getInstance(props, auth);
      //session.setDebug(true);

      MimeMessage msg = new MimeMessage(session);
      msg.setText(body);
      msg.setSubject(subject);
      msg.setFrom(new InternetAddress(username));
      msg.addRecipient(Message.RecipientType.TO, new InternetAddress(to));
      
      Transport.send(msg);
    }
    catch (Exception mex)
    {
      mex.printStackTrace();
    }
  }

  /**
   * SimpleAuthenticator is used to do simple authentication when the SMTP
   * server requires it.
   */
  private class SMTPAuthenticator
    extends javax.mail.Authenticator
  {

    public PasswordAuthentication getPasswordAuthentication()
    {
      return new PasswordAuthentication(username, password);
    }
  }
}
