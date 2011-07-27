include (U2Gui.pri)

# Input
HEADERS += src/AppSettingsGUI.h \
           src/MainWindow.h \
           src/Notification.h \
           src/NotificationWidget.h \
           src/ObjectViewModel.h \
           src/ObjectViewTasks.h \
           src/OpenViewTask.h \
           src/PluginViewer.h \
           src/ProjectParsing.h \
           src/ProjectView.h \
           src/UnloadDocumentTask.h \
           src/util/AddNewDocumentDialogController.h \
           src/util/AddNewDocumentDialogImpl.h \
           src/util/AnnotationSettingsDialogController.h \
           src/util/AnnotationSettingsDialogImpl.h \
           src/util/AuthenticationDialog.h \
           src/util/BaseDocumentFormatConfigurators.h \
           src/util/CopyDocumentDialogController.h \
           src/util/CreateAnnotationDialog.h \
           src/util/CreateAnnotationWidgetController.h \
           src/util/CreateDocumentFromTextDialogController.h \
           src/util/CreateObjectRelationDialogController.h \
           src/util/DialogUtils.h \
           src/util/DocumentFormatComboboxController.h \
           src/util/DownloadRemoteFileDialog.h \
           src/util/EditQualifierDialog.h \
           src/util/EditSequenceDialogController.h \
           src/util/ExportImageDialog.h \
           src/util/GlassView.h \
           src/util/GObjectComboBoxController.h \
           src/util/GraphUtils.h \
           src/util/GScrollBar.h \
           src/util/GUIUtils.h \
           src/util/HBar.h \
           src/util/LastUsedDirHelper.h \
           src/util/ObjectViewTreeController.h \
           src/util/PositionSelector.h \
           src/util/ProjectDocumentComboBoxController.h \
           src/util/ProjectTreeController.h \
           src/util/ProjectTreeItemSelectorDialog.h \
           src/util/ProjectTreeItemSelectorDialogImpl.h \
           src/util/RangeSelector.h \
           src/util/RemovePartFromSequenceDialogController.h \
           src/util/SaveDocumentGroupController.h \
           src/util/ScaleBar.h \
           src/util/ScriptEditorDialog.h \
           src/util/SeqPasterWidgetController.h \
           src/util/TextEditorDialog.h \
           src/util/TreeWidgetUtils.h \
           src/util/logview/LogView.h
FORMS += src/util/ui/AddNewDocumentDialog.ui \
         src/util/ui/AnnotationSettingsDialog.ui \
         src/util/ui/AuthentificationDialog.ui \
         src/util/ui/CopyDocumentDialog.ui \
         src/util/ui/CreateAnnotationWidget.ui \
         src/util/ui/CreateDocumentFromTextDialog.ui \
         src/util/ui/CreateObjectRelationDialog.ui \
         src/util/ui/DownloadRemoteFileDialog.ui \
         src/util/ui/EditQualifierDialog.ui \
         src/util/ui/EditSequenceDialog.ui \
         src/util/ui/ExportImageDialog.ui \
         src/util/ui/FormatSettingsDialog.ui \
         src/util/ui/MultipartDocFormatConfiguratorWidget.ui \
         src/util/ui/ProjectTreeItemSelectorDialogBase.ui \
         src/util/ui/RemovePartFromSequenceDialog.ui \
         src/util/ui/ScriptEditorDialog.ui \
         src/util/ui/SeqPasterWidget.ui \
         src/util/ui/TextEditorDialog.ui
SOURCES += src/MainWindow.cpp \
           src/Notification.cpp \
           src/NotificationWidget.cpp \
           src/ObjectViewModel.cpp \
           src/ObjectViewTasks.cpp \
           src/OpenViewTask.cpp \
           src/ProjectParsing.cpp \
           src/UnloadDocumentTask.cpp \
           src/util/AddNewDocumentDialogImpl.cpp \
           src/util/AnnotationSettingsDialogImpl.cpp \
           src/util/AuthenticationDialog.cpp \
           src/util/BaseDocumentFormatConfigurators.cpp \
           src/util/CopyDocumentDialogController.cpp \
           src/util/CreateAnnotationDialog.cpp \
           src/util/CreateAnnotationWidgetController.cpp \
           src/util/CreateDocumentFromTextDialogController.cpp \
           src/util/CreateObjectRelationDialogController.cpp \
           src/util/DialogUtils.cpp \
           src/util/DocumentFormatComboboxController.cpp \
           src/util/DownloadRemoteFileDialog.cpp \
           src/util/EditQualifierDialog.cpp \
           src/util/EditSequenceDialogController.cpp \
           src/util/ExportImageDialog.cpp \
           src/util/GlassView.cpp \
           src/util/GObjectComboBoxController.cpp \
           src/util/GraphUtils.cpp \
           src/util/GScrollBar.cpp \
           src/util/GUIUtils.cpp \
           src/util/HBar.cpp \
           src/util/LastUsedDirHelper.cpp \
           src/util/ObjectViewTreeController.cpp \
           src/util/PositionSelector.cpp \
           src/util/ProjectDocumentComboBoxController.cpp \
           src/util/ProjectTreeController.cpp \
           src/util/ProjectTreeItemSelectorDialog.cpp \
           src/util/RangeSelector.cpp \
           src/util/RemovePartFromSequenceDialogController.cpp \
           src/util/SaveDocumentGroupController.cpp \
           src/util/ScaleBar.cpp \
           src/util/ScriptEditorDialog.cpp \
           src/util/SeqPasterWidgetController.cpp \
           src/util/TextEditorDialog.cpp \
           src/util/TreeWidgetUtils.cpp \
           src/util/logview/LogView.cpp
RESOURCES += U2Gui.qrc
TRANSLATIONS += transl/chinese.ts \
                transl/czech.ts \
                transl/english.ts \
                transl/russian.ts
