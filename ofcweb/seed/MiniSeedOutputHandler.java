/*
 * MiniSeedOutputHandler.java
 *
 * Created on February 8, 2006, 10:34 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package seed;

/** This provides the interface that needs to be implemented for a class
 * which can get Mini-Seed output (primarily from the compressor in RawToMiniSeed).
 *
 * @author davidketchum
 */
public interface MiniSeedOutputHandler {
    public void putbuf(byte [] b, int size);
    public void close();
}
