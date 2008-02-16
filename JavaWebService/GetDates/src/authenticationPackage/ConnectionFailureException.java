package authenticationPackage;

import java.sql.SQLException;

public class ConnectionFailureException
  extends SQLException
{
private String message;
private boolean transientFailure;
  public ConnectionFailureException(boolean value)
  {
  transientFailure = value;
  }

  public void setMessage(String message)
  {
    this.message = message;
  }

  public String get_message()
  {
    return message;
  }

  public void setTransientFailure(boolean transientFailure)
  {
    this.transientFailure = transientFailure;
  }

  public boolean isTransientFailure()
  {
    return transientFailure;
  }
}
