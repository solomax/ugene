/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
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

#ifndef ExtrenalProcessWorker_h__
#define ExtrenalProcessWorker_h__


#include <U2Core/Task.h> 
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>
#include <U2Lang/ExternalToolCfg.h>
#include <U2Lang/WorkflowEnv.h>

namespace U2 {
namespace LocalWorkflow {


class ExternalProcessWorker: public BaseWorker {
    Q_OBJECT
public:
    ExternalProcessWorker(Actor *a): BaseWorker(a, false), output(NULL) {
        ExternalToolCfgRegistry * reg = WorkflowEnv::getExternalCfgRegistry();
        cfg = reg->getConfigByName(actor->getProto()->getId());
        commandLine = cfg->cmdLine;
        done = false;
        busy = false;
    }
    bool isReady();
    bool isDone();
    Task* tick();
    void init();
    void cleanup();

private slots:
    void sl_onTaskFinishied();
    
private:
    const QString generateAndCreateURL(const QString &extention, const QString &name);

    CommunicationChannel *output;
    QList<CommunicationChannel *> inputs;
    QString commandLine;
    ExternalProcessConfig *cfg;

    QStringList inputUrls;
    QMap<QString, DataConfig> outputUrls;
    bool done;
    bool busy;
};

class ExternalProcessWorkerFactory: public DomainFactory {
public:
    ExternalProcessWorkerFactory(QString name) : DomainFactory(name) {}
    static bool init(ExternalProcessConfig * cfg);
    virtual Worker* createWorker(Actor* a) {return new ExternalProcessWorker(a);}
};

class ExternalProcessWorkerPrompter: public PrompterBase<ExternalProcessWorkerPrompter> {
    Q_OBJECT
public:
    ExternalProcessWorkerPrompter(Actor *p = NULL): PrompterBase<ExternalProcessWorkerPrompter>(p) {}
    QString composeRichDoc();
};

class LaunchExternalToolTask: public Task {
    Q_OBJECT
public:
    LaunchExternalToolTask(const QString &execString);
    void run();

private:
    QString execString;
};


}
}


#endif // ExtrenalProcessWorker_h__
