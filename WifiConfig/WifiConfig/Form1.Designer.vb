<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class FrmConfig
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.cmdConfig = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txtSSID = New System.Windows.Forms.TextBox()
        Me.txtPassword = New System.Windows.Forms.TextBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.lblPortName = New System.Windows.Forms.Label()
        Me.CmbSecurity = New System.Windows.Forms.ComboBox()
        Me.SuspendLayout()
        '
        'cmdConfig
        '
        Me.cmdConfig.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.cmdConfig.Location = New System.Drawing.Point(38, 267)
        Me.cmdConfig.Name = "cmdConfig"
        Me.cmdConfig.Size = New System.Drawing.Size(654, 57)
        Me.cmdConfig.TabIndex = 0
        Me.cmdConfig.Text = "Configure"
        Me.cmdConfig.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(31, 83)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(189, 39)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "WIFI SSID:"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(31, 141)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(176, 39)
        Me.Label2.TabIndex = 2
        Me.Label2.Text = "Password:"
        '
        'txtSSID
        '
        Me.txtSSID.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtSSID.Location = New System.Drawing.Point(297, 77)
        Me.txtSSID.Name = "txtSSID"
        Me.txtSSID.Size = New System.Drawing.Size(395, 45)
        Me.txtSSID.TabIndex = 3
        '
        'txtPassword
        '
        Me.txtPassword.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtPassword.Location = New System.Drawing.Point(297, 135)
        Me.txtPassword.Name = "txtPassword"
        Me.txtPassword.Size = New System.Drawing.Size(395, 45)
        Me.txtPassword.TabIndex = 4
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label3.Location = New System.Drawing.Point(31, 198)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(234, 39)
        Me.Label3.TabIndex = 6
        Me.Label3.Text = "Security Type:"
        '
        'lblPortName
        '
        Me.lblPortName.AutoSize = True
        Me.lblPortName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblPortName.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.857143!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblPortName.Location = New System.Drawing.Point(189, 26)
        Me.lblPortName.Name = "lblPortName"
        Me.lblPortName.Size = New System.Drawing.Size(311, 31)
        Me.lblPortName.TabIndex = 7
        Me.lblPortName.Text = "RobotArm is on Port: COM3"
        '
        'CmbSecurity
        '
        Me.CmbSecurity.Font = New System.Drawing.Font("Microsoft Sans Serif", 14.14286!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CmbSecurity.FormattingEnabled = True
        Me.CmbSecurity.Items.AddRange(New Object() {"WPA/WPA2", "WEP"})
        Me.CmbSecurity.Location = New System.Drawing.Point(297, 195)
        Me.CmbSecurity.Name = "CmbSecurity"
        Me.CmbSecurity.Size = New System.Drawing.Size(395, 46)
        Me.CmbSecurity.TabIndex = 8
        '
        'FrmConfig
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(11.0!, 24.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(714, 357)
        Me.Controls.Add(Me.CmbSecurity)
        Me.Controls.Add(Me.lblPortName)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.txtPassword)
        Me.Controls.Add(Me.txtSSID)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmdConfig)
        Me.Name = "FrmConfig"
        Me.Text = "Configure the WIFI"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents cmdConfig As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txtSSID As System.Windows.Forms.TextBox
    Friend WithEvents txtPassword As System.Windows.Forms.TextBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents lblPortName As System.Windows.Forms.Label
    Friend WithEvents CmbSecurity As System.Windows.Forms.ComboBox

End Class
