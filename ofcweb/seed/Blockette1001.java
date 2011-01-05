/*
 * Blockette1001.java
 *
 * Created on November 21, 2006, 3:31 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package seed;
import java.nio.ByteBuffer;
/**
 * The blockette 1000 has :
 * 0  i*2 blockette type  is 1000
 * 2  i*2 Next Blockette byte number (offset to next blockette)
 * 4  Timing quality (0-100) 
 * 5  usecs - in addition to the  other time
 * 6  reserved
 * 7  frame count (the number of frames in compression section that are occupied)
 *
 * @author davidketchum
 */
public class Blockette1001 {
  byte [] buf;
  ByteBuffer bb;
  /** Creates a new instance of Blockette1001
   * @param b a buffer with 8 raw bytes for blockette 1001*/
  public Blockette1001(byte [] b) {
    buf = new byte[b.length];
    System.arraycopy(b,0, buf, 0, b.length);
    bb = ByteBuffer.wrap(buf);
  }
  public void reload(byte [] b) {System.arraycopy(b, 0, buf, 0, b.length);}
  /** get timingQuality
   *@return The timing quality (normally 0-101)*/
  public int getTimingQuality() {return ((int) buf[4]) & 0xff;}
  /** get used frame count
   *@return The number of frames marked used in this B1001*/
  public int getFrameCount() {return (int) buf[7];}
  /** get microseconds correction
   *@return the microsecs correction from b1001*/
  public int getUSecs() {return (int) buf[5];}
  /** set timing Quality 
   *@param b A timing quality byte normally (0-101) */
  public void setTimingQuality(byte b) {buf[4] = b;}
  /** set the frame count used
   *@param len The # of frames used in this b1001's data record */
  public void setFrameCount(int len) { buf[7] = (byte) len;}
  /** set the microseconds
   *@param usecs Set the microseconds part of the B1001*/
  public void setUSecs(int usecs) {buf[5] = (byte) usecs;}
  /** get raw bytes for this b1001
   *@return The 8 raw bytes for this b1001*/
  public byte [] getBytes() {return buf;}
  /** string rep
   *@return A string representing the data in this B1001.*/
  public String toString(){return "(MI:tQ="+Util.leftPad(""+getTimingQuality(),3)+" #fr="+getFrameCount()+" u="+getUSecs()+")";}
}
