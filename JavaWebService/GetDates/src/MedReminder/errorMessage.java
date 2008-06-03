package MedReminder;

public class errorMessage
{
  private int errorCode;
  private boolean success;
  private String errorMessage;
  private boolean retryNeeded;

  public errorMessage()
  {
    errorCode = 0;
    errorMessage = "";
    success = true;
    retryNeeded = false;
  }

  public errorMessage(String ErrorMessage, boolean Success, 
                      boolean RetryNeeded, int ErrorCode)
  {
    errorMessage = ErrorMessage;
    success = Success;
    retryNeeded = RetryNeeded;
    errorCode = ErrorCode;
  }
  
  /**
   * When there is an error
   * @param message
   */
  public errorMessage(String message)
  {
    this.errorMessage = message;
    this.errorCode = -1;
    this.success = false;
    this.retryNeeded = false;
  }

/**
   * When there is an error with a specific code
   * @param message
   * @param errorCode
   */
  public errorMessage(String message, int errorCode)
  {
    this.errorMessage = message;
    this.errorCode = errorCode;
    this.success = (errorCode == 0);
    
    this.retryNeeded = false;
  }
  


  public void setErrorCode(int errorCode)
  {
    this.errorCode = errorCode;
  }

  public int getErrorCode()
  {
    return errorCode;
  }

  public void setSuccess(boolean success)
  {
    this.success = success;
  }

  public boolean isSuccess()
  {
    return success;
  }

  public void setErrorMessage(String errorMessage)
  {
    this.errorMessage = errorMessage;
  }

  public String getErrorMessage()
  {
    return errorMessage;
  }

  public void setRetryNeeded(boolean retryNeeded)
  {
    this.retryNeeded = retryNeeded;
  }

  public boolean isRetryNeeded()
  {
    return retryNeeded;
  }
}
