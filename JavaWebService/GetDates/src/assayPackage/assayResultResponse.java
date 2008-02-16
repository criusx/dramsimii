package assayPackage;

public class assayResultResponse
{
  private boolean success;
  private boolean needRetry;

  public assayResultResponse()
  {
    success = false;
    needRetry = true;
  }

  public assayResultResponse(boolean Success, boolean NeedRetry)
  {
    success = Success;
    needRetry = NeedRetry;
  }

  public void setSuccess(boolean success)
  {
    this.success = success;
  }

  public boolean isSuccess()
  {
    return success;
  }

  public void setNeedRetry(boolean needRetry)
  {
    this.needRetry = needRetry;
  }

  public boolean isNeedRetry()
  {
    return needRetry;
  }
}
