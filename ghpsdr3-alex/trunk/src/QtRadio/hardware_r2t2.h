#if !defined __HARDWARE_R2T2_H__
#define      __HARDWARE_R2T2_H__

#include "hardware.h"

class QRadioButton;
class QCheckBox;

class HardwareR2t2: public DlgHardware
{ 
   Q_OBJECT

public:
   HardwareR2t2 (Connection *pC, QWidget *p);
   ~HardwareR2t2 ();

private:
   QSignalMapper *attMapper;
   int attenuatorVal;
   QSignalMapper *antMapper;
   int antennaVal;
   QSignalMapper *preselMapper;
   int preselVal;
   QRadioButton *psel[16];
   QCheckBox *preamp;
   int preampVal;
   

private slots:
   void attClicked(int state);
   void antClicked(int n);
   void preselClicked(int n);
   void preampChanged(int n);
   void processAnswer (QStringList);
};

#endif

