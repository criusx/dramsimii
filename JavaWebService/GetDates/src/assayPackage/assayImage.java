package assayPackage;

public class assayImage
{
  byte[] image;

  public assayImage()
  {
  }

  public assayImage(byte[] value)
  {
    image = value;
  }

  public void setImage(byte[] image)
  {
    this.image = image;
  }

  public byte[] getImage()
  {
    return image;
  }
}
