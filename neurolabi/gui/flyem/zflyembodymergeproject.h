#ifndef ZFLYEMBODYMERGEPROJECT_H
#define ZFLYEMBODYMERGEPROJECT_H

#include <QObject>
#include <QList>
#include "dvid/zdvidtarget.h"
#include "tz_stdint.h"
#include "zstackobjectselector.h"
#include "zsharedpointer.h"
#include "zstackviewparam.h"
#include "neutube.h"

class ZStackFrame;
class ZFlyEmBodyMergeFrame;
class Z3DWindow;
class ZStackObject;
class ZSwcTree;
class ZObject3dScan;
class ZFlyEmNeuron;
class ZIntPoint;
class ZStackDocReader;
class ZArray;
class Z3DWindow;
class ZStackDoc;
class ZFlyEmBodyMerger;
class ZWidgetMessage;
class ZDvidInfo;
class ZProgressSignal;
//class ZStackViewParam;

class ZFlyEmBodyMergeProject : public QObject
{
  Q_OBJECT
public:
  explicit ZFlyEmBodyMergeProject(QObject *parent = 0);
  ~ZFlyEmBodyMergeProject();

  void test();
  void clear();

  bool hasDataFrame() const;
  void setDataFrame(ZStackFrame *frame);
  void closeDataFrame();

  void setDocData(ZStackDocReader &reader);

  void loadSlice(int x, int y, int z, int width, int height);
  void loadSliceFunc(int x, int y, int z, int width, int height);

  void changeDvidNode(const std::string &newUuid);

  inline const ZDvidTarget& getDvidTarget() const {
    return m_dvidTarget;
  }

  inline void setDvidTarget(const ZDvidTarget &target) {
    m_dvidTarget = target;
  }

  inline ZFlyEmBodyMergeFrame* getDataFrame() {
    return m_dataFrame;
  }

  //Obsolete functions
  int getSelectedBodyId() const;
  void addSelected(uint64_t label);
  void removeSelected(uint64_t label);

  bool lockNode(const QString &message);
  std::string createVersionBranch();

  int getCurrentZ() const;

  void setDocument(ZSharedPointer<ZStackDoc> doc);
  ZStackDoc* getDocument() const;
  template<typename T>
  T* getDocument() const;

  void syncWithDvid();

  void setSelection(
      const std::set<uint64_t> &selected, NeuTube::EBodyLabelType labelType);

  //void setSelectionFromOriginal(const std::set<uint64_t> &selected);

  Z3DWindow* getBodyWindow() { return m_bodyWindow; }
  void closeBodyWindow();

  uint64_t getMappedBodyId(uint64_t label) const;

  void emitMessage(const QString msg, bool appending = true);
  void emitError(const QString msg, bool appending = true);

  ZProgressSignal* getProgressSignal() const;

signals:
  void progressAdvanced(double dp);
  void progressStarted();
  void progressEnded();
  void newDocReady(ZStackDocReader *reader, bool readyForPaint);
//  void originalLabelUpdated(ZArray *label);
  void originalLabelUpdated(ZArray *label, QSet<uint64_t> *selectedSet);
  void selectionChanged(ZStackObjectSelector selector);
  void selectionChanged();
  void bodyMerged(QList<uint64_t> objLabelList);
  void splitSent(ZDvidTarget target, int bodyId);
  void locating2DViewTriggered(ZStackViewParam);
  void dvidLabelChanged();
  void messageGenerated(const ZWidgetMessage&);

  /*
  void messageGenerated(QString, bool appending = true);
  void errorGenerated(QString, bool appending = true);
  */

public slots:
  void viewGrayscale(const ZIntPoint &offset, int width, int height);
  void loadGrayscaleFunc(int z, bool lowres);
  void shallowClear();
  void mergeBody();
  void setLoadingLabel(bool state);
  void uploadResult();
  void saveMergeOperation();
  void update3DBodyView(const ZStackObjectSelector &selector);
  void update3DBodyView();
  void update3DBodyViewDeep();
  void showBody3d();
  void detachBodyWindow();
  void notifySplit();
  void highlightSelectedObject(bool hl);
  void update3DBodyViewPlane();

private:
  ZFlyEmBodyMerger* getBodyMerger() const;
  void clearBodyMerger();
  void update3DBodyViewPlane(const ZDvidInfo &dvidInfo);
  void update3DBodyViewBox(const ZDvidInfo &dvidInfo);
  void uploadResultFunc();
  //void updateSelection();

private:
  ZFlyEmBodyMergeFrame *m_dataFrame;
  ZSharedPointer<ZStackDoc> m_doc;
  Z3DWindow *m_bodyWindow;
  ZDvidTarget m_dvidTarget;
//  Z3DWindow *m_resultWindow;
//  Z3DWindow *m_quickViewWindow;
//  ZFlyEmBookmarkArray m_bookmarkArray;
//  std::vector<ZStackObject*> m_bookmarkDecoration;
//  bool m_isBookmarkVisible;
  bool m_showingBodyMask;
  QSet<uint64_t> m_selectedOriginal; //the set of original ids of selected bodies
//  QSet<uint64_t> m_currentSelected;

  ZProgressSignal *m_progressSignal;
};

template <typename T>
T* ZFlyEmBodyMergeProject::getDocument() const
{
  return dynamic_cast<T*>(getDocument());
}

#endif // ZFLYEMBODYMERGEPROJECT_H
