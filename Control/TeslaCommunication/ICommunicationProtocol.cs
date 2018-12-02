﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.Text;
using TeslaCommunication.Packets;

namespace TeslaCommunication
{
    [Serializable]
    public class HardwareState
    {
        public bool enabledF1;
        public bool enabledF2;
        public bool enabledF3;
        public bool enabledF4;
        public bool ledLight;

        bool byteToBool(byte b)
        {
            if (b > 0)
                return true;
            return false;
        }

        public HardwareState(StateStruct state)
        {
            this.enabledF1 = byteToBool(state.enabled_f1);
            this.enabledF2 = byteToBool(state.enabled_f2);
            this.enabledF3 = byteToBool(state.enabled_f3);
            this.enabledF4 = byteToBool(state.enabled_f4);
            this.ledLight = byteToBool(state.led_light);
        }
    }

    // Define a service contract.
    [ServiceContract(Namespace = "http://STM32TeslaCommunication")]
    public interface ICommunicationProtocol
    {
        [OperationContract]
        bool Connect(string comPortName);

        [OperationContract]
        bool IsConnected();

        [OperationContract]
        void Disconnect();

        [OperationContract]
        void ClearQueues();

        [OperationContract]
        void setEnabled(int fNum, bool enabled);

        [OperationContract]
        HardwareState getHardwareState();
    }
}