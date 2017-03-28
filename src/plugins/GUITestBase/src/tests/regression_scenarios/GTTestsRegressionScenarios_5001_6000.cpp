/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <QApplication>
#include <QFile>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QTableView>
#include <QTableWidget>
#include <QWebElement>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>

#include <U2Gui/ToolsMenu.h>

#include <U2View/ADVConstants.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/DetView.h>
#include <U2View/MSAEditorTreeViewer.h>
#include <U2View/MaGraphOverview.h>

#include <base_dialogs/DefaultDialogFiller.h>
#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTAction.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMenu.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSlider.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTTabWidget.h>
#include <primitives/GTTableView.h>
#include <primitives/GTTextEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTClipboard.h>
#include <system/GTFile.h>
#include <utils/GTThread.h>
#include <utils/GTUtilsDialog.h>

#include "GTTestsRegressionScenarios_5001_6000.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsCircularView.h"
#include "GTUtilsDashboard.h"
#include "GTUtilsDocument.h"
#include "GTUtilsExternalTools.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsNotifications.h"
#include "GTUtilsOptionPanelMSA.h"
#include "GTUtilsOptionPanelSequenceView.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsPcr.h"
#include "GTUtilsPhyTree.h"
#include "GTUtilsPrimerLibrary.h"
#include "GTUtilsProject.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsStartPage.h"
#include "GTUtilsTask.h"
#include "GTUtilsTaskTreeView.h"
#include "GTUtilsWizard.h"
#include "GTUtilsWorkflowDesigner.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/PredictSecondaryStructureDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_assembly/ExportCoverageDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/DistanceMatrixDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/GenerateAlignmentProfileDialogFiller.h"
#include "runnables/ugene/corelibs/U2View/ov_msa/LicenseAgreementDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/DotPlotDialogFiller.h"
#include "runnables/ugene/plugins/dotplot/BuildDotPlotDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/ConstructMoleculeDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/DigestSequenceDialogFiller.h"
#include "runnables/ugene/plugins/enzymes/FindEnzymesDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/FormatDBDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/BlastAllSupportDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SnpEffDatabaseDialogFiller.h"
#include "runnables/ugene/plugins/external_tools/SpadesGenomeAssemblyDialogFiller.h"
#include "runnables/ugene/plugins/pcr/ImportPrimersDialogFiller.h"
#include "runnables/ugene/plugins_3rdparty/umuscle/MuscleDialogFiller.h"
#include "runnables/ugene/ugeneui/DocumentFormatSelectorDialogFiller.h"
#include "runnables/ugene/ugeneui/SaveProjectDialogFiller.h"
#include "runnables/ugene/ugeneui/SequenceReadingModeSelectorDialogFiller.h"

namespace U2 {

namespace GUITest_regression_scenarios {
using namespace HI;

GUI_TEST_CLASS_DEFINITION(test_5004) {
    //1. Open file _common_data/scenarios/_regression/5004/short.fa
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/5004", "short.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget *sequenceWidget = GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_SET_ERR(NULL != sequenceWidget, "sequenceWidget is not present");

    GTWidget::click(os, sequenceWidget);

    GTLogTracer lt;
    // 2. Show DNA Flexibility graph
    // Expected state: no errors in log
    QWidget *graphAction = GTWidget::findWidget(os, "GraphMenuAction", sequenceWidget, false);
    Runnable *chooser = new PopupChooser(os, QStringList() << "DNA Flexibility");
    GTUtilsDialog::waitForDialog(os, chooser);
    GTWidget::click(os, graphAction);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5012) {
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam3.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
}

GUI_TEST_CLASS_DEFINITION(test_5012_1) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/genbank/JQ040024.1.gb");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Assembly/chrM.fa");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_1.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasError(), "There is no error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5012_2) {
    GTLogTracer l;
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::addSample(os, "Call variants with SAMtools");
    GTUtilsWorkflowDesigner::click(os, "Read Assembly (BAM/SAM)");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam1.sam");

    GTUtilsWorkflowDesigner::createDataset(os);
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/bam/scerevisiae.bam2.sam");

    GTUtilsWorkflowDesigner::click(os, "Read Sequence");

    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir +"_common_data/genbank/pBR322.gb");

    GTUtilsWorkflowDesigner::click(os, "Call Variants");
    GTUtilsWorkflowDesigner::setParameter(os, "Output variants file", QDir(sandBoxDir).absoluteFilePath("test_5012_2.vcf"), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.hasError(), "There is no error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5018) {
#ifdef Q_OS_WIN
    const QString homePlaceholder = "%UserProfile%";
#else
    const QString homePlaceholder = "~";
#endif

//    1. Ensure that there is no "test_5018.fa" file in the home dir.
    const QString homePath = QDir::homePath();
    if (GTFile::check(os, homePath + "/test_5018.fa")) {
        QFile(homePath + "/test_5018.fa").remove();
        CHECK_SET_ERR(!GTFile::check(os, homePath + "/test_5018.fa"), "File can't be removed");
    }

//    2. Open "data/samples/FASTA/human_T1.fa".
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    3. Call context menu on the sequence object in the Project View, select {Export/Import -> Export sequences...} item.
//    4. Set output path to "~/test_5018.fa" for *nix and "%HOME_DIR%\test_5018.fa" for Windows. Accept the dialog.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export/Import" << "Export sequences..."));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, homePlaceholder + "/test_5018.fa"));
    GTUtilsProjectTreeView::click(os, "human_T1 (UCSC April 2002 chr7:115977709-117855134)", Qt::RightButton);

    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: "test_5018.fa" appears in the home dir.
    CHECK_SET_ERR(GTFile::check(os, homePath + "/test_5018.fa"), "File was not created");
    GTUtilsDialog::waitForDialog(os, new MessageBoxNoToAllOrNo(os));
    QFile(homePath + "/test_5018.fa").remove();
    GTGlobals::sleep(5000);
}

GUI_TEST_CLASS_DEFINITION(test_5027_1) {
    //1. Open preferences and set memory limit per task 500000MB
    //2. Open WD and compose next scheme "File list" -> "SnpEff annotation and filtration"
    //3. Run schema.
    //Expected state : there is problem on dashboard "A problem occurred during allocating memory for running SnpEff."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int memValue)
            : memValue(memValue) {}
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);

            QSpinBox* memSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "memorySpinBox"));
            CHECK_SET_ERR(memSpinBox != NULL, "No memorySpinBox");
            GTSpinBox::setValue(os, memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemorySetter(500000)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
    GTThread::waitForMainThread();
    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWebView::findElement(os, GTUtilsDashboard::getDashboard(os), "A problem occurred during allocating memory for running SnpEff.");
}

GUI_TEST_CLASS_DEFINITION(test_5027_2) {
    //1. Open preferences and set memory limit per task 512MB
    //2. Open WD and compose next scheme "File list" -> "SnpEff annotation and filtration"
    //3. Run schema.
    //Expected state : there is problem on dashboard "There is not enough memory to complete the SnpEff execution."
    class MemorySetter : public CustomScenario {
    public:
        MemorySetter(int memValue)
            : memValue(memValue) {}
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            AppSettingsDialogFiller::openTab(os, AppSettingsDialogFiller::Resourses);

            QSpinBox* memSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "memorySpinBox"));
            CHECK_SET_ERR(memSpinBox != NULL, "No memorySpinBox");
            GTSpinBox::setValue(os, memSpinBox, memValue, GTGlobals::UseKeyBoard);

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    private:
        int memValue;
    };

    GTUtilsDialog::waitForDialog(os, new AppSettingsDialogFiller(os, new MemorySetter(256)));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");
    GTGlobals::sleep();

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "SnpEff");
    GTThread::waitForMainThread();
    GTUtilsWorkflowDesigner::click(os, "Input Variations File");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + "_common_data/vcf/valid.vcf");

    GTUtilsWorkflowDesigner::click(os, "Annotate and Predict Effects with SnpEff");
    GTUtilsDialog::waitForDialog(os, new SnpEffDatabaseDialogFiller(os, "hg19"));
    GTUtilsWorkflowDesigner::setParameter(os, "Genome", QVariant(), GTUtilsWorkflowDesigner::customDialogSelector);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTWebView::findElement(os, GTUtilsDashboard::getDashboard(os), "There is not enough memory to complete the SnpEff execution.");
}

GUI_TEST_CLASS_DEFINITION(test_5029) {
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Plugins...");
    GTGlobals::sleep();
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(GTWidget::findWidget(os,"treeWidget"));
    int numPlugins = tree->topLevelItemCount();
    CHECK_SET_ERR( numPlugins > 10, QString("Not all plugins were loaded. Loaded %1 plugins").arg(numPlugins));
}

GUI_TEST_CLASS_DEFINITION(test_5039) {
    //1. Open "COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Set the consensus type to "Levitsky".
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::General);
    GTGlobals::sleep(200);
    QComboBox *consensusCombo = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "consensusType"));
    CHECK_SET_ERR(consensusCombo != NULL, "consensusCombo is NULL");
    GTComboBox::setIndexWithText(os, consensusCombo, "Levitsky");

    //3. Add an additional sequence from file : "test/_common_data/fasta/amino_ext.fa".
    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/fasta/amino_ext.fa"));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Align sequence to this alignment");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //4. Open the "Export consensus" OP tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::ExportConsensus);

    GTLogTracer l;

    //5. Press "Undo" button.
    GTUtilsMsaEditor::undo(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //6. Press "Redo" button.
    GTUtilsMsaEditor::redo(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state : the tab is successfully updated. No error in log.
    CHECK_SET_ERR(!l.hasError(), "unexpected errors in log");
}

GUI_TEST_CLASS_DEFINITION(test_5052) {
    //1. Open "samples/Genbank/murine.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    //2. Close the opened sequence view.
    GTGlobals::FindOptions findOptions;
    findOptions.matchPolicy = Qt::MatchContains;
    GTUtilsMdi::closeWindow(os, "NC_", findOptions);
    //3. Click "murine.gb" on Start Page.
    GTUtilsStartPage::clickResentDocument(os, "murine.gb");
    //Expected: The file is loaded, the view is opened.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(GTUtilsDocument::isDocumentLoaded(os, "murine.gb"), "The file is not loaded");
    QString title = GTUtilsMdi::activeWindowTitle(os);
    CHECK_SET_ERR(title.contains("NC_"), "Wrong MDI window is active");
}

GUI_TEST_CLASS_DEFINITION(test_5069) {
//    1. Load workflow "_common_data/regression/5069/crash.uwl".
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5069/crash.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Set "data/samples/Genbank/murine.gb" as input.
    GTUtilsWorkflowDesigner::click(os, "Read Sequence");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, dataDir + "samples/Genbank/murine.gb");

//    3. Launch workflow.
//    Expected state: UGENE doesn't crash.
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const bool areThereProblems = GTUtilsDashboard::areThereProblems(os);
    CHECK_SET_ERR(!areThereProblems, "Workflow has finished with problems");
}

GUI_TEST_CLASS_DEFINITION(test_5082) {
    GTLogTracer l;
    // 1. Open "_common_data/clustal/big.aln".
    GTFileDialog::openFile(os, testDir + "_common_data/clustal/big.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // 2. Align it with MUSCLE.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Align" << "Align with MUSCLE..."));
    GTUtilsDialog::waitForDialog(os, new MuscleDialogFiller(os));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    // Expected: Error notification appears with a correct human readable error. There is a error in log wit memory requirements.
    GTUtilsNotifications::waitForNotification(os, true, "There is not enough memory to align these sequences with MUSCLE.");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(l.checkMessage("Not enough resources for the task, resource name:"), "No default error in log");
}

GUI_TEST_CLASS_DEFINITION(test_5090) {
//    1. Open "_common_data/genbank/join_complement_ann.gb".
//    Expected state: the file is successfully opened;
//                    a warning appears. It contains next message: "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand."
//                    there are two annotations: 'just_an_annotation' (40..50) and 'join_complement' (join(10..15,20..25)). // the second one should have another location after UGENE-3423 will be done

    GTLogTracer logTracer;
    GTUtilsNotifications::waitForNotification(os, false, "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");

    GTFileDialog::openFile(os, testDir + "_common_data/genbank/join_complement_ann.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, logTracer, "The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.");

    GTUtilsMdi::activateWindow(os, "join_complement_ann [s] A_SEQ_1");

    const QString simpleAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "just_an_annotation");
    CHECK_SET_ERR("40..50" == simpleAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("40..50").arg(simpleAnnRegion));
    const QString joinComplementAnnRegion = GTUtilsAnnotationsTreeView::getAnnotationRegionString(os, "join_complement");
    CHECK_SET_ERR("join(10..15,20..25)" == joinComplementAnnRegion, QString("An incorrect annotation region: expected '%1', got '%2'").arg("join(10..15,20..25)").arg(simpleAnnRegion));
}

GUI_TEST_CLASS_DEFINITION(test_5128) {
    //1. Open any 3D structure.
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");

    //2. Context menu: { Molecular Surface -> * }.
    //3. Select any model.
    //Current state: crash
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Molecular Surface" << "SAS"));
    GTWidget::click(os, GTWidget::findWidget(os, "1-1CF7"), Qt::RightButton);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5137) {
    //    1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW", "COI.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Add big sequence
    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/fasta/", "PF07724_full_family.fa");
    GTUtilsDialog::waitForDialog(os, ob);

    QAbstractButton *align = GTAction::button(os, "Align sequence to this alignment");
    CHECK_SET_ERR(align != NULL, "MSA \"Align sequence to this alignment\" action not found");
    GTWidget::click(os, align);
    GTUtilsNotifications::waitForNotification(os, true, "A problem occurred during adding sequences. The multiple alignment is no more available.");
    GTGlobals::sleep();
    GTUtilsProjectTreeView::click(os, "COI");
    GTKeyboardDriver::keyClick(Qt::Key_Delete);
    GTGlobals::sleep(6000);
}

GUI_TEST_CLASS_DEFINITION(test_5138_1) {
    //1. Open document test/_common_data/scenarios/msa/ma2_gapped.aln
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Join));
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/msa/", "big_aln.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Do MSA area context menu->Statistics->generate distance matrix
    //    Expected state: notification about low memory has appeared
    Runnable* dis = new DistanceMatrixDialogFiller(os, DistanceMatrixDialogFiller::NONE, testDir + "_common_data/scenarios/sandbox/matrix.html");
    GTUtilsDialog::waitForDialog(os, dis);
    Runnable* pop = new PopupChooser(os, QStringList() << MSAE_MENU_STATISTICS << "Generate distance matrix", GTGlobals::UseKey);
    GTUtilsDialog::waitForDialog(os, pop);
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsNotifications::waitForNotification(os, true, "not enough memory");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5138_2) {
    //    1. Open document test/_common_data/clustal/big.aln
    GTFileDialog::openFile(os, testDir + "_common_data/scenarios/_regression/5138", "big_5138.aln");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //    2. Do MSA area context menu->Statistics->generate grid profile
    //    Expected state: notification about low memory has appeared
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << MSAE_MENU_STATISTICS << "Generate grid profile", GTGlobals::UseKey));
    GTUtilsDialog::waitForDialog(os, new GenerateAlignmentProfileDialogFiller(os, true, GenerateAlignmentProfileDialogFiller::NONE,
        testDir + "_common_data/scenarios/sandbox/stat.html"));
    GTMenu::showContextMenu(os, GTUtilsMdi::activeWindow(os));
    GTUtilsNotifications::waitForNotification(os, true, "not enough memory");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5199) {
//    1. Open "data/samples/PDB/1CF7.PDB".
    GTFileDialog::openFile(os, dataDir + "samples/PDB/1CF7.PDB");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    2. Set focus to the first sequence.
    GTWidget::click(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));

//    3. Click "Predict secondary structure" button on the toolbar;
//    4. Select "PsiPred" algorithm.
//    5. Click "OK" button.
//    Expected state: UGENE doesn't crash, 4 results are found.

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "algorithmComboBox", dialog), "PsiPred");
            GTUtilsDialog::waitForDialogWhichMayRunOrNot(os, new LicenseAgreementDialogFiller(os));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
            GTUtilsTaskTreeView::waitTaskFinished(os);

            QTableWidget *resultsTable = GTWidget::findExactWidget<QTableWidget *>(os, "resultsTable", dialog);
            GTGlobals::sleep();
            CHECK_SET_ERR(NULL != resultsTable, "resultsTable is NULL");
            const int resultsCount = resultsTable->rowCount();
            CHECK_SET_ERR(4 == resultsCount, QString("Unexpected results count: expected %1, got %2").arg(4).arg(resultsCount));
            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };

    GTUtilsDialog::waitForDialog(os, new PredictSecondaryStructureDialogFiller(os, new Scenario));
    GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Predict secondary structure");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5208) {
    //    1. Open the library, clear it.
    GTUtilsPrimerLibrary::openLibrary(os);
    GTUtilsPrimerLibrary::clearLibrary(os);

    //    2. Click "Import".
    //    3. Fill the dialog:
    //        Import from: "Local file(s)";
    //        Files: "_common_data/fasta/random_primers.fa"
    //    and accept the dialog.
    class ImportFromMultifasta : public CustomScenario {
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");
            ImportPrimersDialogFiller::setImportTarget(os, ImportPrimersDialogFiller::LocalFiles);
            ImportPrimersDialogFiller::addFile(os, testDir + "_common_data/fasta/random_primers.fa");
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Ok);
        }
    };
    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new ImportPrimersDialogFiller(os, new ImportFromMultifasta));
    GTUtilsPrimerLibrary::clickButton(os, GTUtilsPrimerLibrary::Import);

    //    4. Check log.
    //    Expected state: the library contains four primers, log contains no errors.
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5216) {
    // 1. Connect to the public database
    //GTUtilsSharedDatabaseDocument::connectToUgenePublicDatabase(os);
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTLogTracer lt;
    // 2. Type to the project filter field "acct" then "acctt"
    GTUtilsProjectTreeView::filterProjectSequental(os, QStringList() << "acct" << "accttt", true);
    GTGlobals::sleep(3500);
    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
    //GTUtilsProjectTreeView::filterProject(os, "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
}

GUI_TEST_CLASS_DEFINITION(test_5227) {
    GTUtilsPcr::clearPcrDir(os);

    //1. Open "samples/Genbank/CVU55762.gb".
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/", "CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Open the PCR OP.
    GTWidget::click(os, GTWidget::findWidget(os, "OP_IN_SILICO_PCR"));

    //3. Set next parameters:
    // the first primer : TTCTGGATTCA
    // the first primer mismatches : 15
    // the second primer : CGGGTAG
    // the second primer mismatches : 12
    // 3' perfect match: 10
    // Maximum product : 100 bp
    GTUtilsPcr::setPrimer(os, U2Strand::Direct, "TTCTGGATTCA");
    GTUtilsPcr::setPrimer(os, U2Strand::Complementary, "CGGGTAG");

    GTUtilsPcr::setMismatches(os, U2Strand::Direct, 15);
    GTUtilsPcr::setMismatches(os, U2Strand::Complementary, 12);

    QSpinBox *perfectSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "perfectSpinBox"));
    GTSpinBox::setValue(os, perfectSpinBox, 10, GTGlobals::UseKeyBoard);

    QSpinBox *productSizeSpinBox = dynamic_cast<QSpinBox*>(GTWidget::findWidget(os, "productSizeSpinBox"));
    GTSpinBox::setValue(os, productSizeSpinBox, 100, GTGlobals::UseKeyBoard);

    //4. Find products
    //Expected state: log shouldn't contain errors
    GTLogTracer lt;
    GTWidget::click(os, GTWidget::findWidget(os, "findProductButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
}


GUI_TEST_CLASS_DEFINITION(test_5246) {
    //1. Open file human_t1.fa
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    //2. Show ORFs
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Show ORFs"));
    GTWidget::click(os, GTWidget::findWidget(os, "toggleAutoAnnotationsButton"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidget *widget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    QList<QTreeWidgetItem*> treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(839 == treeItems.size(), "Unexpected annotation count");

    //3. Change amino translation
    GTWidget::click(os, GTWidget::findWidget(os, "ADV_single_sequence_widget_0"));
    GTWidget::click(os, GTWidget::findWidget(os, "AminoToolbarButton", GTWidget::findWidget(os, "ADV_single_sequence_widget_0")));
    QMenu *menu = qobject_cast<QMenu *>(QApplication::activePopupWidget());
    GTMenu::clickMenuItemByName(os, menu, QStringList() << "14. The Alternative Flatworm Mitochondrial Code");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //Expected state: orfs recalculated
    treeItems = GTTreeWidget::getItems(widget->invisibleRootItem());
    CHECK_SET_ERR(2023 == treeItems.size(), "Unexpected annotation count");
}

GUI_TEST_CLASS_DEFINITION(test_5249) {
    // 1. Open file "_common_data/pdb/1atp.pdb"
    // Expected state: no crash and no errors in the log
    GTLogTracer l;

    GTFileDialog::openFile(os, testDir + "_common_data/pdb/1atp.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!l.hasError(), "Error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5268) {
//    1. Open "data/samples/CLUSTALW/COI.aln".
    GTFileDialog::openFile(os, dataDir + "samples/CLUSTALW/COI.aln");

//    2. Create a custom color scheme for the alignment with aan ppropriate alphabet.
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, "test_5268", NewColorSchemeCreator::nucl));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");

//    3. Open "Highlighting" options panel tab.
    GTUtilsOptionPanelMsa::openTab(os, GTUtilsOptionPanelMsa::Highlighting);

//    4. Select the custom color scheme.
    GTUtilsOptionPanelMsa::setColorScheme(os, "test_5268");
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Colors" << "Custom schemes" << "test_5268", PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

//    5. Open {Settings -> Preferences -> Alignment Color Scheme}.
//    6. Change color of the custom color scheme and click ok.
    GTUtilsDialog::waitForDialog(os, new NewColorSchemeCreator(os, "test_5268", NewColorSchemeCreator::nucl, NewColorSchemeCreator::Change));
    GTMenu::clickMainMenuItem(os, QStringList() << "Settings" << "Preferences...");

    GTGlobals::sleep(500);

//    Expected state: the settings dialog closed, new colors are applied for the opened MSA.
    const QString opColorScheme = GTUtilsOptionPanelMsa::getColorScheme(os);
    CHECK_SET_ERR(opColorScheme == "test_5268",
                  QString("An incorrect color scheme is set in option panel: expect '%1', got '%2'")
                  .arg("test_5268").arg(opColorScheme));

    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Colors" << "Custom schemes" << "test_5268", PopupChecker::IsChecked));
    GTUtilsMSAEditorSequenceArea::callContextMenu(os);

    GTGlobals::sleep(500);
}

GUI_TEST_CLASS_DEFINITION(test_5278) {
    //1. Open file PBR322.gb from samples
    GTFileDialog::openFile(os, dataDir + "samples/Genbank", "PBR322.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    //2. Find next restriction sites "AaaI" and "AagI"
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "AaaI" << "AagI"));
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsNotifications::waitForNotification(os, false);
    //3. Open report and be sure fragments sorted by length (longest first)
    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Cloning" << "Digest into fragments...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTGlobals::sleep();
    QTextEdit *textEdit = dynamic_cast<QTextEdit*>(GTWidget::findWidget(os, "reportTextEdit", GTUtilsMdi::activeWindow(os)));
    CHECK_SET_ERR(textEdit->toPlainText().contains("1:    From AaaI (944) To AagI (24) - 3442 bp "), "Expected message is not found in the report text");
}

GUI_TEST_CLASS_DEFINITION(test_5295) {
//    1. Open "_common_data/pdb/Helix.pdb".
    GTFileDialog::openFile(os, testDir + "_common_data/pdb/Helix.pdb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: UGENE doesn't crash, the 3d structure is shown.
    QWidget *biostructWidget = GTWidget::findWidget(os, "1-");
    const QImage image1 = GTWidget::getImage(os, biostructWidget);
    QSet<QRgb> colors;
    for (int i = 0; i < image1.width(); i++) {
        for (int j = 0; j < image1.height(); j++) {
            colors << image1.pixel(i, j);
        }
    }
    CHECK_SET_ERR(colors.size() > 1, "Biostruct was not drawn");

//    2. Call a context menu, open "Render Style" submenu.
//    Expected state: "Ball-and-Stick" renderer is selected.
    GTUtilsDialog::waitForDialog(os, new PopupCheckerByText(os, QStringList() << "Render Style" << "Ball-and-Stick", PopupChecker::CheckOptions(PopupChecker::IsChecked)));
    GTWidget::click(os, biostructWidget, Qt::RightButton);

//    3. Select "Model" renderer. Select "Ball-and-Stick" again.
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Render Style" << "Model"));
    GTWidget::click(os, biostructWidget, Qt::RightButton);
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Render Style" << "Ball-and-Stick"));
    GTWidget::click(os, biostructWidget, Qt::RightButton);

//    Expected state: UGENE doesn't crash, the 3d structure is shown.
    const QImage image2 = GTWidget::getImage(os, biostructWidget);
    colors.clear();
    for (int i = 0; i < image2.width(); i++) {
        for (int j = 0; j < image2.height(); j++) {
            colors << image2.pixel(i, j);
        }
    }
    CHECK_SET_ERR(colors.size() > 1, "Biostruct was not drawn after renderer change");
}

GUI_TEST_CLASS_DEFINITION(test_5314) {
    //1. Open "data/samples/Genbank/CVU55762.gb".
    //2. Search any enzyme on the whole sequence.
    //3. Open "data/samples/ABIF/A01.abi".
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/CVU55762.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const QStringList defaultEnzymes = QStringList() << "ClaI";
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "ADV_MENU_ANALYSE" << "Find restriction sites"));
    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, defaultEnzymes));
    GTMenu::showContextMenu(os, GTUtilsSequenceView::getSeqWidgetByNumber(os));
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTLogTracer lt;
    GTFileDialog::openFile(os, testDir + "_common_data/abif/A01.abi");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();
    CHECK_SET_ERR(!lt.hasError(), "Log shouldn't contain errors");
}

GUI_TEST_CLASS_DEFINITION(test_5346) {
    // 1. Open WD
    // 2. Create the workflow: File List - FastQC Quality Control
    // 3. Set empty input file
    // Expected state: there is an error "The input file is empty"
    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    QString emptyFile = sandBoxDir + "test_5346_empty";
    GTFile::create(os, emptyFile);
    WorkflowProcessItem* fileList = GTUtilsWorkflowDesigner::addElement(os, "File List");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, emptyFile);

    WorkflowProcessItem* fastqc = GTUtilsWorkflowDesigner::addElement(os, "FastQC Quality Control");
    GTUtilsWorkflowDesigner::connect(os, fileList, fastqc);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsLog::checkContainsError(os, l, QString("The input file '%1' is empty.").arg(QFileInfo(emptyFile).absoluteFilePath()));
}

GUI_TEST_CLASS_DEFINITION(test_5352) {
//    1. Open WD
//    2. Open any sample (e.g. Align with MUSCLE)
//    3. Remove some elements and set input data
//    4. Run the workflow
//    5. Click "Load dashboard workflow"
//    Expected state: message box about workflow modification appears
//    6. Click "Close without saving"
//    Expected state: the launched workflow is loaded successfully, no errors

    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::addSample(os, "Align sequences with MUSCLE");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::removeItem(os, "Align with MUSCLE");

    WorkflowProcessItem* read = GTUtilsWorkflowDesigner::getWorker(os, "Read alignment");
    WorkflowProcessItem* write = GTUtilsWorkflowDesigner::getWorker(os, "Write alignment");
    GTUtilsWorkflowDesigner::connect(os, read, write);

    GTUtilsWorkflowDesigner::click(os, "Read alignment");
    GTUtilsWorkflowDesigner::addInputFile(os, "Read alignment", dataDir + "samples/CLUSTALW/COI.aln");

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Discard));
    HIWebElement element = GTUtilsDashboard::findElement(os, "", "BUTTON");
    GTUtilsDashboard::click(os, element);

    CHECK_SET_ERR(!l.hasError(), "There is and error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5356) {
//    1. Open WD
//    2. Create workflow: "Read FASTQ" --> "Cut Adapter" --> "FastQC"
//       (open _common_data/regression/5356/cutadapter_and_trim.uwl)
//    3. Set input data:
//       reads - _common_data/regression/5356/reads.fastq
//       adapter file -  _common_data/regression/5356/adapter.fa
//    4. Run the workflow
//    Expected state: no errors in the log (empty sequences were skipped by CutAdapter)

    GTLogTracer l;

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/regression/5356/cutadapt_and_trim.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::addInputFile(os, "Read FASTQ Files with Reads 1", testDir + "_common_data/regression/5356/reads.fastq");

    GTUtilsWorkflowDesigner::click(os, "Cut Adapter");
    GTUtilsWorkflowDesigner::setParameter(os, "FASTA file with 3' adapters", QDir(testDir + "_common_data/regression/5356/adapter.fa").absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output directory", "Custom", GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Custom directory", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!l.hasError(), "There is an error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5360) {
    //1. Open scheme _common_data / scenarios / _regression / 5360 / 5360.uwl
    //
    //2. Set input fastq file located with path containing non ASCII symbols
    //
    //3. Run workflow
    //Expected state : workflow runs without errors.
    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "_common_data/scenarios/_regression/5360/5360.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::click(os, "Read FASTQ Files with Reads");
    GTUtilsWorkflowDesigner::setDatasetInputFile(os, testDir + QString::fromUtf8("_common_data/scenarios/_regression/5360/папка/риды.fastq"), true);

    GTLogTracer lt;
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    CHECK_SET_ERR(!lt.hasError(), "There is an error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5363_1) {
//    1. {Tools --> BLAST --> BLAST make database}
//    2. Set murine.gb as input file
//    3. Check nucleotide radiobutton
//    4. Create database
//    Expected state: database was successfully created
//    5. Open murine.gb
//    6. {Analyze --> Query with local BLAST}
//    7. Select the created database and accept the dialog
//    Expected state: blast annotations were found and the annotations locations are equal to 'hit-from' and 'hit-to' qualifier values

    FormatDBSupportRunDialogFiller::Parameters parametersDB;
    parametersDB.inputFilePath = dataDir + "/samples/Genbank/murine.gb";
    parametersDB.outputDirPath = sandBoxDir;
    GTUtilsDialog::waitForDialog(os, new FormatDBSupportRunDialogFiller(os, parametersDB));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "BLAST" << "BLAST make database...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    BlastAllSupportDialogFiller::Parameters parametersSearch;
    parametersSearch.runBlast = true;
    parametersSearch.dbPath = sandBoxDir + "/murine.nin";

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(parametersSearch, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* treeItem = GTUtilsAnnotationsTreeView::findItem(os, "blast result");
    CHECK_SET_ERR(treeItem != NULL, "blast result annotations not found");
    bool ok;
    int hitFrom = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-to", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-to qualifier value");

    int hitTo = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-from", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-from qualifier value");

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(hitFrom, hitTo - hitFrom)),
                  QString("Cannot find blast result [%1, %2]").arg(hitFrom).arg(hitTo));
}

GUI_TEST_CLASS_DEFINITION(test_5363_2) {
//    1. {Tools --> BLAST --> BLAST+ make database}
//    2. Set murine.gb as input file
//    3. Check nucleotide radiobutton
//    4. Create database
//    Expected state: database was successfully created
//    5. Open murine.gb
//    6. {Analyze --> Query with local BLAST+}
//    7. Select the created database and accept the dialog
//    Expected state: blast annotations were found and the annotations locations are equal to 'hit-from' and 'hit-to' qualifier values

    FormatDBSupportRunDialogFiller::Parameters parametersDB;
    parametersDB.inputFilePath = dataDir + "/samples/Genbank/murine.gb";
    parametersDB.outputDirPath = sandBoxDir;
    GTUtilsDialog::waitForDialog(os, new FormatDBSupportRunDialogFiller(os, parametersDB));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "BLAST" << "BLAST+ make database...");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    BlastAllSupportDialogFiller::Parameters parametersSearch;
    parametersSearch.runBlast = true;
    parametersSearch.dbPath = sandBoxDir + "/murine.nin";

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(parametersSearch, os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST+...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeWidgetItem* treeItem = GTUtilsAnnotationsTreeView::findItem(os, "blast result");
    CHECK_SET_ERR(treeItem != NULL, "blast result annotations not found");
    bool ok;
    int hitFrom = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-to", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-to qualifier value");

    int hitTo = GTUtilsAnnotationsTreeView::getQualifierValue(os, "hit-from", treeItem).toInt(&ok);
    CHECK_SET_ERR(ok, "Cannot get hit-from qualifier value");

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "blast result", U2Region(hitFrom, hitTo - hitFrom)),
                  QString("Cannot find blast result [%1, %2]").arg(hitFrom).arg(hitTo));
}

GUI_TEST_CLASS_DEFINITION(test_5367) {
//    1. Open "_common_data/bam/accepted_hits_with_gaps.bam"
//    2. Export coverage in 'Per base' format
//    Expected state: gaps are not considered "to cover, the result file is qual to "_common_data/bam/accepted_hits_with_gaps_coverage.txt"

    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "/test_5367.ugenedb"));
    GTFileDialog::openFile(os, testDir + "_common_data/bam/accepted_hits_with_gaps.bam");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QList<ExportCoverageDialogFiller::Action> actions;
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::SetFormat, "Per base");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::EnterFilePath, QDir(sandBoxDir).absolutePath() + "/test_5367_coverage.txt");
    actions << ExportCoverageDialogFiller::Action(ExportCoverageDialogFiller::ClickOk, QVariant());

    GTUtilsDialog::waitForDialog(os, new ExportCoverageDialogFiller(os, actions) );
    GTUtilsDialog::waitForDialog(os, new PopupChooserByText(os, QStringList() << "Export coverage..."));
    GTUtilsAssemblyBrowser::callContextMenu(os);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTFile::equals(os, sandBoxDir + "/test_5367_coverage.txt", testDir + "/_common_data/bam/accepted_hits_with_gaps_coverage.txt"), "Exported coverage is wrong!");
}

GUI_TEST_CLASS_DEFINITION(test_5377) {
//    1. Open file "_common_data/genbank/70Bp_new.gb".
//    2. Search for restriction site HinFI.
//    3. Digest into fragments, then reconstruct the original molecule.
//    Expected state: the result sequence is equal to the original sequence. Fragments annotations have the same positions and lengths.
    GTFileDialog::openFile(os, testDir + "_common_data/genbank/70Bp_new.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new FindEnzymesDialogFiller(os, QStringList() << "HinfI"));
    GTWidget::click(os, GTToolbar::getWidgetForActionName(os, GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI), "Find restriction sites"));
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new DigestSequenceDialogFiller(os));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Cloning" << "Digest into fragments...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 1", U2Region(36, 35)), "Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 2", U2Region(1, 24)), "Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "Fragment 3", U2Region(28, 5)), "Fragment 3 is incorrect or not found");

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "activeModalWidget is NULL");

            GTWidget::click(os, GTWidget::findWidget(os, "takeAllButton"));

            QTreeWidget *tree = dynamic_cast<QTreeWidget*>(GTWidget::findWidget(os, "molConstructWidget"));
            GTTreeWidget::click(os, GTTreeWidget::findItem(os, tree, "Blunt"));

            GTWidget::click(os, GTWidget::findWidget(os, "downButton"));
            GTWidget::click(os, GTWidget::findWidget(os, "downButton"));

            QTabWidget* tabWidget = GTWidget::findExactWidget<QTabWidget*>(os, "tabWidget", dialog);
            CHECK_SET_ERR(tabWidget != NULL, "tabWidget not found");
            GTTabWidget::clickTab(os, tabWidget, "Output");

            QLineEdit* linEdit = GTWidget::findExactWidget<QLineEdit*>(os, "filePathEdit");
            CHECK_SET_ERR(linEdit != NULL, "filePathEdit not found");
            GTLineEdit::setText(os, linEdit, QFileInfo(sandBoxDir + "test_5377").absoluteFilePath());

            GTUtilsDialog::clickButtonBox(os, QApplication::activeModalWidget(), QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new ConstructMoleculeDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Cloning" << "Construct molecule...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(GTUtilsSequenceView::getSeqWidgetByNumber(os)->getSequenceLength() == 70, "The result length of the constructed molecule is wrong");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 1", U2Region(36, 35)), "Constructed molecule: Fragment 1 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 2", U2Region(1, 24)), "Constructed molecule: Fragment 2 is incorrect or not found");
    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::findRegion(os, "A sequence Fragment 3", U2Region(28, 5)), "Constructed molecule: Fragment 3 is incorrect or not found");
}

GUI_TEST_CLASS_DEFINITION(test_5371) {
    //1. Open bam assembly with index with path containing non ASCII symbols
    //Expected state: assembly opened successfully

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, sandBoxDir + "5371.bam.ugenedb"));

    GTFileDialogUtils *ob = new GTFileDialogUtils(os, testDir + "_common_data/scenarios/_regression/5371/папка/", "асс ссембли.bam", GTFileDialogUtils::Open,
        GTGlobals::UseKey , GTFileDialogUtils::CopyPaste);
    GTUtilsDialog::waitForDialog(os, ob);

    ob->openFileDialog();
    GTThread::waitForMainThread();
    GTGlobals::sleep(100);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5412) {
//    1. Open "/_common_data/reads/wrong_order/align_bwa_mem.uwl"
//    2. Set input data: e_coli_mess_1.fastq nd e_coli_mess_2.fastq (the directory from step 1)
//    3. Reference: "/_common_data/e_coli/NC_008253.fa"
//    4. Set requiered output parameters
//    5. Set "Filter unpaired reads" to false
//    6. Run workflow
//    Expected state: error - BWA MEM tool exits with code 1
//    7. Go back to the workflow and set the filter parameter back to true
//    8. Run the workflow
//    Expected state: there is a warning about filtered reads

    GTUtilsWorkflowDesigner::openWorkflowDesigner(os);

    GTUtilsWorkflowDesigner::loadWorkflow(os, testDir + "/_common_data/reads/wrong_order/align_bwa_mem.uwl");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsWorkflowDesigner::addInputFile(os, "File List 1", testDir + "/_common_data/reads/wrong_order/e_coli_mess_1.fastq");
    GTUtilsWorkflowDesigner::addInputFile(os, "File List 2", testDir + "/_common_data/reads/wrong_order/e_coli_mess_2.fastq");

    GTUtilsWorkflowDesigner::click(os, "Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Output directory", QDir(sandBoxDir).absolutePath(), GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file name", "test_5412", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Reference genome", testDir + "/_common_data/e_coli/NC_008253.fa", GTUtilsWorkflowDesigner::textValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Filter unpaired reads", false, GTUtilsWorkflowDesigner::comboValue);

    GTLogTracer l;
    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    CHECK_SET_ERR(l.checkMessage("exited with code 1"), "No message about failed start of BWA MEM");

    GTToolbar::clickButtonByTooltipOnToolbar(os, "mwtoolbar_activemdi", "Show workflow");

    GTUtilsWorkflowDesigner::click(os, "Align Reads with BWA MEM");
    GTUtilsWorkflowDesigner::setParameter(os, "Filter unpaired reads", true, GTUtilsWorkflowDesigner::comboValue);
    GTUtilsWorkflowDesigner::setParameter(os, "Output file name", "test_5412_1", GTUtilsWorkflowDesigner::textValue);

    GTUtilsWorkflowDesigner::runWorkflow(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(l.checkMessage("5 pairs are complete, 6 reads without a pair were found in files"), "No message about filtered reads");
}

GUI_TEST_CLASS_DEFINITION(test_5417) {
    //      1. Open "data/samples/Genbank/murine.gb".
    //      2. Open "data/samples/Genbank/srs.gb".
    //      3. Build doplot with theese files and try to save it.
    //      Expected state: warning message ox appeared
    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os));
    Runnable *filler2 = new BuildDotPlotFiller(os, dataDir + "samples/Genbank/sars.gb", dataDir + "samples/Genbank/murine.gb");
    GTUtilsDialog::waitForDialog(os, filler2);

    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "Build dotplot...");

    GTLogTracer lt;
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Dotplot" << "Save/Load" << "Save"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Ok));
    GTMenu::showContextMenu(os, GTWidget::findWidget(os, "dotplot widget"));
    CHECK_SET_ERR(!lt.hasError(), "There is error in the log");
}

GUI_TEST_CLASS_DEFINITION(test_5425) {
    //1. Open de novo assembly dialog
    //2. Fill it and run
    //3. Open dialog again
    //Expected state: all settings except files with reads was saved from previous run
    class SpadesDialogSettingsChecker : public SpadesGenomeAssemblyDialogFiller {
    public:
        SpadesDialogSettingsChecker(HI::GUITestOpStatus &os, QString lib, QString datasetType, QString runningMode,
            QString kmerSizes, int numThreads, int memLimit) : SpadesGenomeAssemblyDialogFiller(os, lib, QStringList(), QStringList(), "",
            datasetType, runningMode, kmerSizes, numThreads, memLimit) {}
        virtual void commonScenario() {
            QWidget* dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(dialog, "activeModalWidget is NULL");

            QComboBox* combo = GTWidget::findExactWidget<QComboBox*>(os, "modeCombo", dialog);
            CHECK_SET_ERR(combo->currentText() == runningMode, "running mode doesn't match");

            combo = GTWidget::findExactWidget<QComboBox*>(os, "typeCombo", dialog);
            CHECK_SET_ERR(combo->currentText() == datasetType, "type mode doesn't match");

            QLineEdit* lineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "kmerEdit", dialog);
            CHECK_SET_ERR(lineEdit->text() == kmerSizes, "kmer doesn't match");

            QSpinBox* spinbox = GTWidget::findExactWidget<QSpinBox*>(os, "memlimitSpin", dialog);
            CHECK_SET_ERR(spinbox->text() == QString::number(memLimit), "memlimit doesn't match");

            spinbox = GTWidget::findExactWidget<QSpinBox*>(os, "numThreadsSpinbox", dialog);
            CHECK_SET_ERR(spinbox->text() == QString::number(numThreads), "threads doesn't match");

            combo = GTWidget::findExactWidget<QComboBox*>(os, "libraryComboBox", dialog);
            CHECK_SET_ERR(combo->currentText() == library, QString("library doesn't match, expected %1, actual:%2.").arg(library).arg(combo->currentText()));

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
        }
    };
    GTUtilsDialog::waitForDialog(os, new SpadesGenomeAssemblyDialogFiller(os, "Paired-end (Interlaced)", QStringList() << testDir + "_common_data/cmdline/external-tool-support/spades/ecoli_1K_1.fq",
        QStringList(), sandBoxDir, "Single Cell", "Error correction only", "aaaaa", 1, 228));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Genome de novo assembly...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new SpadesDialogSettingsChecker(os, "Paired-end (Interlaced)", "Single Cell", "Error correction only", "aaaaa", 1, 228));
    GTMenu::clickMainMenuItem(os, QStringList() << "Tools" << "NGS data analysis" << "Genome de novo assembly...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5469) {
    // 1. Open two different GenBank sequences in one Sequence view.
    // 2. Select two different annotations (one from the first sequence, and one from the second sequence) using the "Ctrl" keyboard button.
    // Extected state: there is no crash
    GTFileDialog::openFile(os, dataDir + "samples/Genbank/sars.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTFileDialog::openFile(os, dataDir + "samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsProjectTreeView::click(os, "NC_004718");
    QWidget* seqView = GTUtilsSequenceView::getSeqWidgetByNumber(os);
    CHECK_SET_ERR(seqView != NULL, "Fail to get sequence view");
    QPoint p = GTWidget::getWidgetCenter(os, seqView);
    p.setX(p.x() + 1);
    p.setY(p.y() + 1);
    GTMouseDriver::dragAndDrop(GTMouseDriver::getMousePosition(), p);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTKeyboardDriver::keyPress(Qt::Key_Control);
    GTUtilsSequenceView::clickAnnotationDet(os, "misc_feature", 2);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsSequenceView::clickAnnotationDet(os, "5'UTR", 1, 1);
    GTKeyboardDriver::keyRelease(Qt::Key_Control);

    CHECK_SET_ERR(GTUtilsAnnotationsTreeView::getAllSelectedItems(os).size() == 2, "Wrong number of selected annotations");
}

GUI_TEST_CLASS_DEFINITION(test_5499) {
//    1. Open txt file (_common_data/text/text.txt).
//    Expected state: "Select correct document format" dialog appears
//    2. Select "Choose format manually" with the default ABIF format.
//    3. Click Ok.
    GTLogTracer logTracer;

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "_common_data/text/text.txt"));
    GTUtilsDialog::waitForDialog(os, new DocumentFormatSelectorDialogFiller(os, "ABIF"));
    GTUtilsDialog::waitForDialog(os, new SequenceReadingModeSelectorDialogFiller(os, SequenceReadingModeSelectorDialogFiller::Separate));
    GTMenu::clickMainMenuItem(os, QStringList() << "File" << "Open as...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

//    Expected state: the message about "not ABIF format" appears, UGENE doesn't crash.
    GTUtilsLog::checkContainsError(os, logTracer, "Not a valid ABIF file");
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5517) {
    //1. Open sequence
    //2. Open build dotplot dialog
    //3. Check both checkboxes direct and invert repeats search
    //Expected state: UGENE not crashed
    GTFileDialog::openFile(os, dataDir + "samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep();

    GTUtilsDialog::waitForDialog(os, new DotPlotFiller(os, 100, 0, true));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Build dotplot...", GTGlobals::UseMouse);
    GTGlobals::sleep();
}

GUI_TEST_CLASS_DEFINITION(test_5520_1) {
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "/_common_data/cmdline/external-tool-support/blastall/sars_middle.nhr"));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton"));

            QRadioButton* rbNewTable = GTWidget::findExactWidget<QRadioButton*>(os, "rbCreateNewTable");
            CHECK_SET_ERR(rbNewTable != NULL, "rbCreateNewTable not found");
            GTRadioButton::click(os, rbNewTable);
            GTGlobals::sleep();

            QLineEdit* leTablePath = GTWidget::findExactWidget<QLineEdit*>(os, "leNewTablePath");
            CHECK_SET_ERR(leTablePath != NULL, "leNewTablePath not found");
            GTLineEdit::setText(os, leTablePath, sandBoxDir + "/test_5520_1.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST...");
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

GUI_TEST_CLASS_DEFINITION(test_5520_2) {
    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    class Scenario : public CustomScenario {
    public:
        void run(HI::GUITestOpStatus &os) {
            QWidget *dialog = QApplication::activeModalWidget();
            CHECK_SET_ERR(NULL != dialog, "Active modal widget is NULL");

            GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, testDir + "/_common_data/cmdline/external-tool-support/blastall/sars_middle.nhr"));
            GTWidget::click(os, GTWidget::findWidget(os, "selectDatabasePushButton"));

            QRadioButton* rbNewTable = GTWidget::findExactWidget<QRadioButton*>(os, "rbCreateNewTable");
            CHECK_SET_ERR(rbNewTable != NULL, "rbCreateNewTable not found");
            GTRadioButton::click(os, rbNewTable);
            GTGlobals::sleep();

            QLineEdit* leTablePath = GTWidget::findExactWidget<QLineEdit*>(os, "leNewTablePath");
            CHECK_SET_ERR(leTablePath != NULL, "leNewTablePath not found");
            GTLineEdit::setText(os, leTablePath, sandBoxDir + "/test_5520_2.gb");

            GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
        }
    };

    GTUtilsDialog::waitForDialog(os, new BlastAllSupportDialogFiller(os, new Scenario()));
    GTMenu::clickMainMenuItem(os, QStringList() << "Actions" << "Analyze" << "Query with local BLAST+...");
    GTUtilsTaskTreeView::waitTaskFinished(os);

}

} // namespace GUITest_regression_scenarios

} // namespace U2
