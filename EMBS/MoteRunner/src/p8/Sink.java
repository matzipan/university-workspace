package p8;

import com.ibm.saguaro.system.*;

public class Sink {
	private final static byte INIT_PACKET = 0x1;
	private final static byte ACK_PACKET = 0x2;
	private final static byte FINAL_PACKET = 0x4;
    private static Radio radioDev = new Radio();
    private static byte[] xmit = new byte[10];
    private static byte[] sequence = new byte[10];
    private static int messageSequenceCounter = 0;
    private static int sequenceItemIndex = 0;
	private static Timer ledTimer;
    private static long ledDelay;

	static {
		ledDelay = Time.toTickSpan(Time.MILLISECS, 1000);
        
        // Setup a periodic timer callback for transmissions
        ledTimer = new Timer();
        ledTimer.setCallback(new TimerEvent(null){
				public void invoke(byte param, long time){
                    Sink.invokeSequenceItemHandling(param, time);
                }
        });
               
	 	// Open the default radio
        radioDev.open(Radio.DID, null, 0, 0);
        // Set the PAN ID to 0x32 and the short address to 0x31
        radioDev.setPanId(0x32, true);
        radioDev.setShortAddr(0x31);

        // Prepare beacon frame with source addressing
        xmit = new byte[10];
        xmit[0] = Radio.FCF_BEACON;
        xmit[1] = Radio.FCA_SRC_SADDR;
        Util.set16le(xmit, 3, 0x32);
        Util.set16le(xmit, 5, 0x31);

        radioDev.setChannel((byte)10);
        
        radioDev.setRxHandler(new DevCallback(null){
        	public int invoke (int flags, byte[] data, int len, int info, long time) {
				return Sink.onReceive(flags, data, len, info, time);
			}
		});
        
        Sink.sendPacket(INIT_PACKET); 
        
        radioDev.startRx(Device.RX4EVER, 0, 0);
	}
	
	private static int onReceive(int flags, byte[] data, int len, int info, long time) {
		if (data == null) { // marks end of reception period
			// re-enable reception for a very long time
			radioDev.startRx(Device.RX4EVER, 0, 0);
			return 0;
		}
		
		if(data[7] == messageSequenceCounter+1) {
			messageSequenceCounter = data[7] + 1;

			if(data[8] == FINAL_PACKET) {
				sequenceItemIndex = 0;
				Sink.handleSequenceItem();
			} else {
				sequence[sequenceItemIndex] = data[9];
				
				sequenceItemIndex++;
				
				Sink.sendPacket(ACK_PACKET);
			}
		} 		
		
	
		return 0;
	}
		
	private static void sendPacket(byte packet) {
		xmit[7] = (byte) messageSequenceCounter;
		xmit[8] = packet;
		
        radioDev.transmit(Device.ASAP|Radio.TXMODE_CCA, xmit, 0, 10, 0);           		
	}
	
	private static void handleSequenceItem() {
		LED.setState((byte)0, (byte)0);
		LED.setState((byte)1, (byte)0);
		LED.setState((byte)2, (byte)0);
		
		if(sequenceItemIndex < sequence.length) {			
			LED.setState(sequence[sequenceItemIndex], (byte)1);
			
	        ledTimer.setAlarmBySpan(ledDelay);
	        sequenceItemIndex++;
		}
	}
	
	private static void invokeSequenceItemHandling(byte param, long time) {
		Sink.handleSequenceItem();
	}
}
