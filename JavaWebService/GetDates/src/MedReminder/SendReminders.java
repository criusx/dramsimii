package MedReminder;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.Timestamp;

import java.util.Date;
import java.util.Properties;

import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

import oracle.jdbc.driver.OracleConnection;
import oracle.jdbc.pool.OracleDataSource;


public class SendReminders
{
  public SendReminders()
  {
  }

  public static void main(String[] args)
  {
    sendEmailReminders();

  }

  public static void sendEmailReminders()
  {
    OracleDataSource ods;
    OracleConnection conn = null;

    try
    {
      ods = new OracleDataSource();
      ods.setURL("jdbc:oracle:thin:rfid/rfid2006@id1.gentag.com:1521:gentagdb");
      conn = (OracleConnection) ods.getConnection();
      conn.setAutoCommit(false);

      PreparedStatement ps2 = 
        conn.prepareStatement("SELECT NAME FROM DRUGDATA WHERE DRUGID = ?");

      PreparedStatement ps = 
        conn.prepareStatement("SELECT LASTDOSE, FREQUENCY, DOSE, EMAIL, MED FROM MEDREMINDERS WHERE 1=1", 
                              ResultSet.TYPE_SCROLL_INSENSITIVE, 
                              ResultSet.CONCUR_UPDATABLE);

      ResultSet rs = ps.executeQuery();

      SQLWarning x = ps.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      x = rs.getWarnings();

      if (x != null)
        System.out.println(x.getMessage());

      while (rs.next())
      {

        Timestamp lastDose = rs.getTimestamp("LASTDOSE");
        lastDose.setTime(lastDose.getTime() + 
                         rs.getInt("FREQUENCY") * 60000);

        Date currentDate = new Date();
        if (lastDose.before(currentDate))
        {
          ps2.setString(1, rs.getString("MED"));
          ResultSet rs2 = ps2.executeQuery();
          if (rs2.next())
          {
            sendEmail(rs.getString("EMAIL"), "Med Reminder", 
                      "It is time to take " + rs.getInt("DOSE") + 
                      " mg of " + rs2.getString("NAME"));

            Timestamp newTS = new Timestamp(currentDate.getTime());

            rs.updateTimestamp(1, newTS);
            rs.updateRow();

          }
          rs2.close();
        }
      }

      rs.close();

      ps.close();
      ps2.close();
      System.out.println((new Date()).getTime());
      conn.commit();

    }
    catch (SQLException e)
    {
      e.printStackTrace();
      try
      {
        conn.rollback();
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
      }
      return;
    }
    catch (AddressException e)
    {
      e.printStackTrace();
      try
      {
        conn.rollback();
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
      }
      return;
    }
    catch (MessagingException e)
    {
      e.printStackTrace();
      try
      {
        conn.rollback();
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
      }
      return;
    }
    finally
    {
      // close up the connection
      try
      {
        if (conn != null)
        {
          conn.close();
        }
      }
      catch (SQLException ex)
      {
        ex.printStackTrace();
        try
        {
          if (conn != null)
            conn.close();
        }
        catch (SQLException exc)
        {
          exc.printStackTrace();
        }
      }
    }
  }

  private static final String username = "medreminder@gentag.com";
  private static final String hostName = "smtp.gmail.com";

  public static void sendEmail(String to, String subject, String body)
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

    // Authenticator auth = new SMTPAuthenticator();
    Sender auth = new Sender();
    Session session = Session.getInstance(props, auth);
    //session.setDebug(true);

    MimeMessage msg = new MimeMessage(session);
    msg.setText(body);
    msg.setSubject(subject);
    msg.setFrom(new InternetAddress(username));
    msg.addRecipient(Message.RecipientType.TO, new InternetAddress(to));

    Transport.send(msg);

  }


}
