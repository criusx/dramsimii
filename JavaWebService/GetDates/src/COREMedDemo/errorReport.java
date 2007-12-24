package COREMedDemo;

public class errorReport
{
  private int errorCode;
  private String errorMessage;

  public errorReport()
  {
    errorCode = 0;
    errorMessage = "";
  }

  public errorReport(int eC, String eM)
  {
    errorCode = eC;
    errorMessage = eM;
  }


  public void setErrorCode(int errorCode)
  {
    this.errorCode = errorCode;
  }

  public int getErrorCode()
  {
    return errorCode;
  }

  public void setErrorMessage(String errorMessage)
  {
    this.errorMessage = errorMessage;
  }

  public String getErrorMessage()
  {
    return errorMessage;
  }
}
