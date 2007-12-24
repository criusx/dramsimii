package petPackage;

public class errorMessage
{
  boolean success;
  String errorMessage;
  String attribute1;
  String attribute2;

  public errorMessage()
  {
    success = true;
    errorMessage = "Success";
  }

  public errorMessage(String _errorMessage)
  {
    success = false;
    errorMessage = _errorMessage;
  }
  
  public errorMessage(String _errorMessage, String _attrib1, String _attrib2)
  {
    success = true;
    errorMessage = _errorMessage;
    attribute1 = _attrib1;
    attribute2 = _attrib2;
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

  public void setAttribute1(String attribute1)
  {
    this.attribute1 = attribute1;
  }

  public String getAttribute1()
  {
    return attribute1;
  }

  public void setAttribute2(String attribute2)
  {
    this.attribute2 = attribute2;
  }

  public String getAttribute2()
  {
    return attribute2;
  }
}
