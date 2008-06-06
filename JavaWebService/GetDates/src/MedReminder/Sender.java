package MedReminder;

import javax.mail.PasswordAuthentication;

public class Sender
  extends javax.mail.Authenticator
{
  private static final String username = "medreminder@gentag.com";
  private static final String password = "gentagmed2006";


  public Sender()
  {
  }

  /**
   * SimpleAuthenticator is used to do simple authentication when the SMTP
   * server requires it.
   */
  public PasswordAuthentication getPasswordAuthentication()
  {
    return new PasswordAuthentication(username, password);
  }
}
