Public Class FrmConfig
    'constant for UART connection
    Private Const baudRate = 115200
    Private Const dataBits = 8
    Private Const parity = IO.Ports.Parity.None
    Private Const stopBits = IO.Ports.StopBits.One
    'Serialport holds the info about port connection
    Private Serialport As IO.Ports.SerialPort = Nothing
    Function FindDevice() As IO.Ports.SerialPort
        For Each portname As String In IO.Ports.SerialPort.GetPortNames()
            Dim port As IO.Ports.SerialPort = New IO.Ports.SerialPort(portname, baudRate, parity, dataBits, stopBits)
            Try
                port.Open() 'open the port
                port.Write("Who?") 'Discover the Robot Arm
                port.ReadTimeout = 500 'read time-out after 500 ms
                Dim revMessage As String = port.ReadLine() 'read response from a device
                'Check if the port is connected to the Robot Arm
                If (revMessage.Equals(New String("Robot Arm"))) Then
                    'If port connected to the Robot Arm then Return the Port
                    Return port
                End If
            Catch ex As Exception
                port.Close()
            Finally
                port.Close()
            End Try
        Next
        'Return Nothing If cannot find the robot arm
        Return Nothing
    End Function
    Sub disableControl()
        cmdConfig.Enabled = False
        txtPassword.Enabled = False
        txtSSID.Enabled = False
        CmbSecurity.Enabled = False
    End Sub
    Sub enableControl()
        cmdConfig.Enabled = True
        txtPassword.Enabled = True
        txtSSID.Enabled = True
        CmbSecurity.Enabled = True
    End Sub
    Private Sub txtPassword_TextChanged(sender As Object, e As EventArgs) Handles txtPassword.TextChanged

    End Sub
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        'Disable control on the form
        disableControl()
        'At Start, Discover the robot arm device
        Serialport = FindDevice()
        'If discover the port then output port number
        If (Serialport IsNot Nothing) Then
            lblPortName.Text = "Robot Arm is on Port: " + Serialport.PortName
            'Enable the controls on the form
            enableControl()
        Else
            lblPortName.Text = "Unable to find the Robot Arm."
        End If
    End Sub

End Class
