object SignalsStateForm: TSignalsStateForm
  Left = 0
  Top = 0
  BorderStyle = bsSizeToolWin
  Caption = #1057#1080#1075#1085#1072#1083#1099
  ClientHeight = 427
  ClientWidth = 323
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = MyPopupMenu
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object eCols: TLabeledEdit
    Left = 158
    Top = 8
    Width = 41
    Height = 21
    EditLabel.Width = 108
    EditLabel.Height = 13
    EditLabel.Caption = #1069#1083#1077#1084#1077#1085#1090#1086#1074' '#1074' '#1089#1090#1086#1083#1073#1094#1077
    LabelPosition = lpLeft
    NumbersOnly = True
    TabOrder = 0
    OnChange = eColsChange
    OnKeyPress = FormKeyPress
  end
  object InOutTimer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = InOutTimerTimer
    Left = 24
    Top = 120
  end
  object MyPopupMenu: TPopupMenu
    Left = 272
    Top = 8
    object nCols: TMenuItem
      Caption = #1069#1083#1077#1084#1077#1085#1090#1086#1074' '#1074' '#1089#1090#1086#1083#1073#1094#1077
    end
    object nShowChannel: TMenuItem
      Caption = #1055#1086#1082#1072#1079#1099#1074#1072#1090#1100' '#1082#1072#1085#1072#1083
      OnClick = nShowChannelClick
    end
    object nExit: TMenuItem
      Caption = #1042#1099#1093#1086#1076
      OnClick = nExitClick
    end
  end
end
