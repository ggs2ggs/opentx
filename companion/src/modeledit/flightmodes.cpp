/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "flightmodes.h"
#include "ui_flightmode.h"
#include "rawitemfilteredmodel.h"
#include "helpers.h"
#include "customdebug.h"

FlightModePanel::FlightModePanel(QWidget * parent, ModelData & model, int phaseIdx, GeneralSettings & generalSettings, Firmware * firmware, RawSwitchFilterItemModel * switchModel):
  ModelPanel(parent, model, generalSettings, firmware),
  ui(new Ui::FlightMode),
  phaseIdx(phaseIdx),
  phase(model.flightModeData[phaseIdx]),
  rawSwitchItemModel(NULL)
{
  ui->setupUi(this);

  ui->labelName->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->labelName->setToolTip(tr("Popup menu available"));
  connect(ui->labelName, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

  board = getCurrentBoard();
  fmCount = firmware->getCapability(FlightModes);
  gvCount = firmware->getCapability(Gvars);
  reCount = firmware->getCapability(RotaryEncoders);
  trimCount = Boards::getCapability(board, Board::NumTrims);

  // Flight mode name
  QRegExp rx(CHAR_FOR_NAMES_REGEX);
  if (fmCount) {
    ui->name->setValidator(new QRegExpValidator(rx, this));
    ui->name->setMaxLength(firmware->getCapability(FlightModesName));
    connect(ui->name, SIGNAL(editingFinished()), this, SLOT(phaseName_editingFinished()));
  }
  else {
    ui->name->setDisabled(true);
  }

  // Flight mode switch
  if (phaseIdx > 0) {
    ui->swtch->setModel(switchModel);
    connect(ui->swtch, SIGNAL(activated(int)), this, SLOT(phaseSwitchChanged(int)));
  }
  else {
    ui->swtch->hide();
  }

  // FadeIn / FadeOut
  if (firmware->getCapability(FlightModesHaveFades)) {
    int scale = firmware->getCapability(SlowScale);
    int range = firmware->getCapability(SlowRange);
    ui->fadeIn->setMaximum(float(range) / scale);
    ui->fadeIn->setSingleStep(1.0 / scale);
    ui->fadeIn->setDecimals((scale == 1 ? 0 :1) );
    connect(ui->fadeIn, SIGNAL(editingFinished()), this, SLOT(phaseFadeIn_editingFinished()));
    ui->fadeOut->setMaximum(float(range) / scale);
    ui->fadeOut->setSingleStep(1.0 / scale);
    ui->fadeOut->setDecimals((scale == 1 ? 0 :1));
    connect(ui->fadeOut, SIGNAL(editingFinished()), this, SLOT(phaseFadeOut_editingFinished()));
  }
  else {
    ui->fadeIn->setDisabled(true);
    ui->fadeOut->setDisabled(true);
  }

  // The trims
  QString labels[CPN_MAX_TRIMS];
  for(int i = 0; i < CPN_MAX_STICKS; i++) {
    labels[i] = firmware->getAnalogInputName(i);
  }
  labels[4] = "T5";   //  TODO firmware function
  labels[5] = "T6";   //  TODO firmware function

  trimsLabel << ui->trim1Label << ui->trim2Label << ui->trim3Label << ui->trim4Label << ui->trim5Label << ui->trim6Label;
  trimsUse << ui->trim1Use << ui->trim2Use << ui->trim3Use << ui->trim4Use << ui->trim5Use << ui->trim6Use;
  trimsValue << ui->trim1Value << ui->trim2Value << ui->trim3Value << ui->trim4Value << ui->trim5Value << ui->trim6Value;
  trimsSlider << ui->trim1Slider << ui->trim2Slider << ui->trim3Slider << ui->trim4Slider << ui->trim5Slider << ui->trim6Slider;

  for (int i = trimCount; i < CPN_MAX_TRIMS; i++) {
    trimsLabel[i]->hide();
    trimsUse[i]->hide();
    trimsValue[i]->hide();
    trimsSlider[i]->hide();
  }

  for (int i = 0; i < trimCount; i++) {
    trimsLabel[i]->setText(labels[CONVERT_MODE(i + 1) - 1]);
    QComboBox * cb = trimsUse[i];
    cb->setProperty("index", i);
    if (IS_HORUS_OR_TARANIS(board)) {
      cb->addItem(tr("Trim disabled"), -1);
    }
    for (int m = 0; m < fmCount; m++) {
      if (m == phaseIdx) {
        cb->addItem(tr("Own Trim"), m * 2);
      }
      else if (phaseIdx > 0) {
        cb->addItem(tr("Use Trim from Flight mode %1").arg(m), m * 2);
        if (IS_HORUS_OR_TARANIS(board)) {
          cb->addItem(tr("Use Trim from Flight mode %1 + Own Trim as an offset").arg(m), m * 2 + 1);
        }
      }
    }
    connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(phaseTrimUse_currentIndexChanged(int)));

    int trimsMax = firmware->getCapability(ExtendedTrimsRange);
    if (trimsMax == 0 || !model.extendedTrims) {
      trimsMax = firmware->getCapability(TrimsRange);
    }

    trimsValue[i]->setProperty("index", i);
    trimsValue[i]->setRange(-trimsMax, +trimsMax);
    connect(trimsValue[i], SIGNAL(valueChanged(int)), this, SLOT(phaseTrim_valueChanged()));

    trimsSlider[i]->setProperty("index", i);
    trimsSlider[i]->setRange(-trimsMax, +trimsMax);
    int chn = CONVERT_MODE(i + 1) - 1;
    if (chn == 2/*TODO constant*/ && model.throttleReversed)
      trimsSlider[i]->setInvertedAppearance(true);
    connect(trimsSlider[i], SIGNAL(valueChanged(int)), this, SLOT(phaseTrimSlider_valueChanged()));
  }

  // Rotary encoders
  if (reCount > 0) {
    QGridLayout *reLayout = new QGridLayout(ui->reGB);
    for (int i = 0; i < reCount; i++) {
      // RE label
      QLabel *label = new QLabel(ui->reGB);
      label->setText(tr("Rotary Encoder %1").arg(i + 1));
      reLayout->addWidget(label, i, 0, 1, 1);
      if (phaseIdx > 0) {
        // RE link to another RE
        reUse[i] = new QComboBox(ui->reGB);
        reUse[i]->setProperty("index", i);
        Helpers::populateGvarUseCB(reUse[i], phaseIdx);
        connect(reUse[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseREUse_currentIndexChanged(int)));
        reLayout->addWidget(reUse[i], i, 1, 1, 1);
      }
      // RE value
      reValues[i] = new QSpinBox(ui->reGB);
      reValues[i]->setProperty("index", i);
      reValues[i]->setMinimum(ENCODER_MIN_VALUE);
      reValues[i]->setMaximum(ENCODER_MAX_VALUE);
      connect(reValues[i], SIGNAL(editingFinished()), this, SLOT(phaseREValue_editingFinished()));
      reLayout->addWidget(reValues[i], i, 2, 1, 1);
    }
  }
  else {
    ui->reGB->hide();
  }

  // GVars
  if (gvCount > 0 && (firmware->getCapability(GvarsFlightModes) || phaseIdx == 0) ) {
    QGridLayout *gvLayout = new QGridLayout(ui->gvGB);

    // Column headers
    int headerCol = 1;

    if (firmware->getCapability(GvarsName)) {
      QLabel *nameLabel = new QLabel(ui->gvGB);
      nameLabel->setText(tr("Name"));
      gvLayout->addWidget(nameLabel, 0, headerCol++, 1, 1);
    }

    if (phaseIdx > 0) {
      QLabel *sourceLabel = new QLabel(ui->gvGB);
      sourceLabel->setText(tr("Value source"));
      gvLayout->addWidget(sourceLabel, 0, headerCol++, 1, 1);
    }

    QLabel *valueLabel = new QLabel(ui->gvGB);
    valueLabel->setText(tr("Value"));
    gvLayout->addWidget(valueLabel, 0, headerCol++, 1, 1);

    if (IS_HORUS_OR_TARANIS(board) && phaseIdx == 0) {
      QLabel *unitLabel = new QLabel(ui->gvGB);
      unitLabel->setText(tr("Unit"));
      gvLayout->addWidget(unitLabel, 0, headerCol++, 1, 1);

      QLabel *precLabel = new QLabel(ui->gvGB);
      precLabel->setText(tr("Prec"));
      gvLayout->addWidget(precLabel, 0, headerCol++, 1, 1);

      QLabel *minLabel = new QLabel(ui->gvGB);
      minLabel->setText(tr("Min"));
      gvLayout->addWidget(minLabel, 0, headerCol++, 1, 1);

      QLabel *maxLabel = new QLabel(ui->gvGB);
      maxLabel->setText(tr("Max"));
      gvLayout->addWidget(maxLabel, 0, headerCol++, 1, 1);

      QLabel *popupLabel = new QLabel(ui->gvGB);
      popupLabel->setText(tr("Popup enabled"));
      gvLayout->addWidget(popupLabel, 0, headerCol++, 1, 1);
    }

    for (int i = 0; i < gvCount; i++) {
      int col = 0;
      // GVar label
      QLabel *label = new QLabel(ui->gvGB);
      label->setText(tr("GVAR%1").arg(i + 1));
      label->setProperty("index", i);
      label->setContextMenuPolicy(Qt::CustomContextMenu);
      label->setToolTip(tr("Popup menu available"));
      connect(label, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(gvOnCustomContextMenuRequested(const QPoint &)));
      gvLayout->addWidget(label, i + 1, col++, 1, 1);
      // GVar name
      int nameLen = firmware->getCapability(GvarsName);
      if (nameLen > 0) {
        gvNames[i] = new QLineEdit(ui->gvGB);
        gvNames[i]->setProperty("index", i);
        gvNames[i]->setMaxLength(nameLen);
        connect(gvNames[i], SIGNAL(editingFinished()), this, SLOT(GVName_editingFinished()));
        gvLayout->addWidget(gvNames[i], i + 1, col++, 1, 1);
      }
      if (phaseIdx > 0) {
        // GVar link to another GVar
        gvUse[i] = new QComboBox(ui->gvGB);
        gvUse[i]->setProperty("index", i);
        Helpers::populateGvarUseCB(gvUse[i], phaseIdx);
        connect(gvUse[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVUse_currentIndexChanged(int)));
        gvLayout->addWidget(gvUse[i], i + 1, col++, 1, 1);
      }
      // GVar value
      gvValues[i] = new QDoubleSpinBox(ui->gvGB);
      gvValues[i]->setProperty("index", i);
      connect(gvValues[i], SIGNAL(editingFinished()), this, SLOT(phaseGVValue_editingFinished()));
      gvLayout->addWidget(gvValues[i], i + 1, col++, 1, 1);

      if (IS_HORUS_OR_TARANIS(board) && phaseIdx == 0) {
        // GVar unit
        gvUnit[i] = new QComboBox(ui->gvGB);
        gvUnit[i]->setProperty("index", i);
        populateGvarUnitCB(gvUnit[i]);
        connect(gvUnit[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVUnit_currentIndexChanged(int)));
        gvLayout->addWidget(gvUnit[i], i + 1, col++, 1, 1);

        // GVar precision
        gvPrec[i] = new QComboBox(ui->gvGB);
        gvPrec[i]->setProperty("index", i);
        populateGvarPrecCB(gvPrec[i]);
        connect(gvPrec[i], SIGNAL(currentIndexChanged(int)), this, SLOT(phaseGVPrec_currentIndexChanged(int)));
        gvLayout->addWidget(gvPrec[i], i + 1, col++, 1, 1);

        // GVar min
        gvMin[i] = new QDoubleSpinBox(ui->gvGB);
        gvMin[i]->setProperty("index", i);
        connect(gvMin[i], SIGNAL(editingFinished()), this, SLOT(phaseGVMin_editingFinished()));
        gvLayout->addWidget(gvMin[i], i + 1, col++, 1, 1);

        // GVar max
        gvMax[i] = new QDoubleSpinBox(ui->gvGB);
        gvMax[i]->setProperty("index", i);
        connect(gvMax[i], SIGNAL(editingFinished()), this, SLOT(phaseGVMax_editingFinished()));
        gvLayout->addWidget(gvMax[i], i + 1, col++, 1, 1);

        // Popups
        gvPopups[i] = new QCheckBox(ui->gvGB);
        gvPopups[i]->setProperty("index", i);
        //gvPopups[i]->setText(tr("Popup enabled"));
        connect(gvPopups[i], SIGNAL(toggled(bool)), this, SLOT(phaseGVPopupToggled(bool)));
        gvLayout->addWidget(gvPopups[i], i + 1, col++, 1, 1);
      }
    }
  }
  else {
    ui->gvGB->hide();
    gvCount = 0;
  }

  disableMouseScrolling();

  update();
}

FlightModePanel::~FlightModePanel()
{
  delete ui;
}

void FlightModePanel::update()
{
  ui->name->setText(phase.name);
  ui->swtch->setCurrentIndex(ui->swtch->findData(phase.swtch.toValue()));

  int scale = firmware->getCapability(SlowScale);
  ui->fadeIn->setValue(float(phase.fadeIn)/scale);
  ui->fadeOut->setValue(float(phase.fadeOut)/scale);

  for (int i = 0; i < getBoardCapability(firmware->getBoard(), Board::NumTrims); i++) {
    trimUpdate(i);
  }

  for (int i = 0; i < gvCount; i++) {
    updateGVar(i);
  }

  for (int i = 0; i < reCount; i++) {
    updateRotaryEncoder(i);
  }
}

void FlightModePanel::updateGVar(int index)
{
  lock = true;
  if (firmware->getCapability(GvarsName) > 0) {
    gvNames[index]->setText(model->gvarData[index].name);
  }
  if (phaseIdx > 0) {
    if (model->isGVarLinked(phaseIdx, index))
      gvUse[index]->setCurrentIndex(phase.gvars[index] - GVAR_MAX_VALUE);
    else
      gvUse[index]->setCurrentIndex(0);
  }
  gvValues[index]->setDisabled(model->isGVarLinked(phaseIdx, index));
  setGVSB(gvValues[index], model->gvarData[index].getMin(), model->gvarData[index].getMax(), model->getGVarFieldValue(phaseIdx, index));
  if (IS_HORUS_OR_TARANIS(board) && phaseIdx == 0) {
    gvUnit[index]->setCurrentIndex(model->gvarData[index].unit);
    gvPrec[index]->setCurrentIndex(model->gvarData[index].prec);
    setGVSB(gvMin[index], GVAR_MIN_VALUE, model->gvarData[index].getMax(), model->gvarData[index].getMin());
    setGVSB(gvMax[index], model->gvarData[index].getMin(), GVAR_MAX_VALUE, model->gvarData[index].getMax());
    gvPopups[index]->setChecked(model->gvarData[index].popup);
  }
  lock = false;
}

void FlightModePanel::setGVSB(QDoubleSpinBox * sb, int min, int max, int val)
{
  int idx = sb->property("index").toInt();
  float mul = model->gvarData[idx].multiplierGet();
  sb->setDecimals(model->gvarData[idx].prec);
  sb->setSingleStep(mul);
  sb->setSuffix(model->gvarData[idx].unitToString());
  sb->setMinimum(min * mul);
  sb->setMaximum(max * mul);
  sb->setValue(val * mul);
}

void FlightModePanel::updateRotaryEncoder(int index)
{
  lock = true;
  if (phaseIdx > 0) {
    if (model->isEncoderLinked(phaseIdx, index))
      reUse[index]->setCurrentIndex(phase.rotaryEncoders[index] - ENCODER_MAX_VALUE);
    else
      reUse[index]->setCurrentIndex(0);
  }
  reValues[index]->setDisabled(model->isEncoderLinked(phaseIdx, index));
  reValues[index]->setValue(model->getEncoderFieldValue(phaseIdx, index));
  lock = false;
}

// TODO: pull units from GVarData
void FlightModePanel::populateGvarUnitCB(QComboBox * cb)
{
  cb->clear();
  cb->addItem("");
  cb->addItem("%");
}

// TODO: pull precision string from GVarData
void FlightModePanel::populateGvarPrecCB(QComboBox * cb)
{
  cb->clear();
  cb->addItem(tr("0._"));
  cb->addItem(tr("0.0"));
}

void FlightModePanel::phaseName_editingFinished()
{
  QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
  strcpy(phase.name, lineEdit->text().toLatin1());
  emit modified();
  emit nameModified();
}

void FlightModePanel::phaseSwitchChanged(int index)
{
  if (!lock) {
    bool ok;
    const RawSwitch rs(ui->swtch->itemData(index).toInt(&ok));
    if (ok && phase.swtch.toValue() != rs.toValue()) {
      phase.swtch = rs;
      emit modified();
    }
  }
}

void FlightModePanel::phaseFadeIn_editingFinished()
{
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int scale = firmware->getCapability(SlowScale);
  phase.fadeIn = round(spinBox->value()*scale);
  emit modified();
}

void FlightModePanel::phaseFadeOut_editingFinished()
{
  QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
  int scale = firmware->getCapability(SlowScale);
  phase.fadeOut = round(spinBox->value()*scale);
  emit modified();
}

void FlightModePanel::trimUpdate(unsigned int trim)
{
  lock = true;
  int chn = CONVERT_MODE(trim + 1) - 1;
  int value = model->getTrimValue(phaseIdx, chn);
  trimsSlider[trim]->setValue(value);
  trimsValue[trim]->setValue(value);
  if (phase.trimMode[chn] < 0) {
    trimsUse[trim]->setCurrentIndex(0);
    trimsValue[trim]->setEnabled(false);
    trimsSlider[trim]->setEnabled(false);
  }
  else {
    if (IS_HORUS_OR_TARANIS(board))
      trimsUse[trim]->setCurrentIndex(1 + 2 * phase.trimRef[chn] + phase.trimMode[chn] - (phase.trimRef[chn] > phaseIdx ? 1 : 0));
    else
      trimsUse[trim]->setCurrentIndex(phase.trimRef[chn]);
    if (phaseIdx == 0 || phase.trimRef[chn] == phaseIdx || (IS_HORUS_OR_TARANIS(board) && phase.trimMode[chn] != 0)) {
      trimsValue[trim]->setEnabled(true);
      trimsSlider[trim]->setEnabled(true);
    }
    else {
      trimsValue[trim]->setEnabled(false);
      trimsSlider[trim]->setEnabled(false);
    }
  }
  lock = false;
}

void FlightModePanel::phaseGVValue_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    phase.gvars[gvar] = spinBox->value() * model->gvarData[gvar].multiplierSet();
    emit modified();
  }
}

void FlightModePanel::GVName_editingFinished()
{
  if (!lock) {
    QLineEdit *lineedit = qobject_cast<QLineEdit*>(sender());
    int gvar = lineedit->property("index").toInt();
    memset(&model->gvarData[gvar].name, 0, sizeof(model->gvarData[gvar].name));
    strcpy(model->gvarData[gvar].name, lineedit->text().toLatin1());
    emit modified();
  }
}

void FlightModePanel::phaseGVUse_currentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    if (index == 0) {
      phase.gvars[gvar] = 0;
    }
    else {
      phase.gvars[gvar] = GVAR_MAX_VALUE + index;
    }
    updateGVar(gvar);
    emit modified();
    lock = false;
  }
}

void FlightModePanel::phaseGVUnit_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    model->gvarData[gvar].unit = index;
    updateGVar(gvar);
    emit modified();
  }
}

void FlightModePanel::phaseGVPrec_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int gvar = comboBox->property("index").toInt();
    model->gvarData[gvar].prec = index;
    updateGVar(gvar);
    emit modified();
  }
}

void FlightModePanel::phaseGVMin_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    model->gvarData[gvar].setMin(spinBox->value());
    if (!model->isGVarLinked(phaseIdx, gvar)) {
      if (model->getGVarFieldValuePrec(phaseIdx, gvar) < spinBox->value()) {
        phase.gvars[gvar] = model->gvarData[gvar].getMin();
      }
    }
    for (int x = 1; x < fmCount; x++) {
      if (!model->isGVarLinked(x, gvar)) {
        if (model->flightModeData[x].gvars[gvar] < model->gvarData[gvar].getMin()) {
          model->flightModeData[x].gvars[gvar] = model->gvarData[gvar].getMin();
        }
      }
    }
    updateGVar(gvar);
    emit modified();
  }
}

void FlightModePanel::phaseGVMax_editingFinished()
{
  if (!lock) {
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(sender());
    int gvar = spinBox->property("index").toInt();
    model->gvarData[gvar].setMax(spinBox->value());
    if (!model->isGVarLinked(phaseIdx, gvar)) {
      if (model->getGVarFieldValuePrec(phaseIdx, gvar) > spinBox->value()) {
        phase.gvars[gvar] = model->gvarData[gvar].getMax();
      }
    }
    for (int x = 1; x < fmCount; x++) {
      if (!model->isGVarLinked(x, gvar)) {
        if (model->flightModeData[x].gvars[gvar] > model->gvarData[gvar].getMax()) {
          model->flightModeData[x].gvars[gvar] = model->gvarData[gvar].getMax();
        }
      }
    }
    updateGVar(gvar);
    emit modified();
  }
}

void FlightModePanel::phaseGVPopupToggled(bool checked)
{
  if (!lock) {
    QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
    int gvar = cb->property("index").toInt();
    model->gvarData[gvar].popup = checked;
    emit modified();
  }
}

void FlightModePanel::phaseREValue_editingFinished()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int re = spinBox->property("index").toInt();
    phase.rotaryEncoders[re] = spinBox->value();
    emit modified();
  }
}

void FlightModePanel::phaseREUse_currentIndexChanged(int index)
{
  if (!lock) {
    lock = true;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int re = comboBox->property("index").toInt();
    if (index == 0) {
      phase.rotaryEncoders[re] = 0;
    }
    else {
      phase.rotaryEncoders[re] = ENCODER_MAX_VALUE + index;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    ;
    }
    update();
    emit modified();
    lock = false;
  }
}

void FlightModePanel::phaseTrimUse_currentIndexChanged(int index)
{
  if (!lock) {
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    int trim = comboBox->property("index").toInt();
    int chn = CONVERT_MODE(trim + 1) - 1;
    int data = comboBox->itemData(index).toInt();
    if (data < 0) {
      phase.trimMode[chn] = -1;
      phase.trimRef[chn] = 0;
      phase.trim[chn] = 0;
    }
    else {
      if (IS_HORUS_OR_TARANIS(board)) {
        phase.trimMode[chn] = data % 2;
        phase.trimRef[chn] = data / 2;
      }
      else {
        phase.trimMode[chn] = 0;
        phase.trimRef[chn] = data / 2;
      }
      phase.trim[chn] = 0;
    }
    trimUpdate(trim);
    emit modified();
  }
}

void FlightModePanel::phaseTrim_valueChanged()
{
  if (!lock) {
    QSpinBox *spinBox = qobject_cast<QSpinBox*>(sender());
    int trim = spinBox->property("index").toInt();
    int chn = CONVERT_MODE(trim + 1) - 1;
    int value = spinBox->value();
    model->setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsSlider[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

void FlightModePanel::phaseTrimSlider_valueChanged()
{
  if (!lock) {
    QSlider *slider = qobject_cast<QSlider*>(sender());
    int trim = slider->property("index").toInt();
    int chn = CONVERT_MODE(trim + 1) - 1;
    int value = slider->value();
    model->setTrimValue(phaseIdx, chn, value);
    lock = true;
    trimsValue[trim]->setValue(value);
    lock = false;
    emit modified();
  }
}

void FlightModePanel::onCustomContextMenuRequested(QPoint pos)
{
  QLabel *label = (QLabel *)sender();
  QPoint globalPos = label->mapToGlobal(pos);

  QMenu contextMenu;
  contextMenu.addAction(CompanionIcon("copy.png"), tr("Copy"), this, SLOT(cmCopy()));
  contextMenu.addAction(CompanionIcon("cut.png"), tr("Cut"), this, SLOT(cmCut()));
  contextMenu.addAction(CompanionIcon("paste.png"), tr("Paste"), this, SLOT(cmPaste()))->setEnabled(hasClipboardData());
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear"), this, SLOT(cmClear()));
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("arrow-right.png"), tr("Insert"), this, SLOT(cmInsert()))->setEnabled(insertAllowed());
  contextMenu.addAction(CompanionIcon("arrow-left.png"), tr("Delete"), this, SLOT(cmDelete()));
  contextMenu.addAction(CompanionIcon("moveup.png"), tr("Move Up"), this, SLOT(cmMoveUp()))->setEnabled(moveUpAllowed());
  contextMenu.addAction(CompanionIcon("movedown.png"), tr("Move Down"), this, SLOT(cmMoveDown()))->setEnabled(moveDownAllowed());
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear All"), this, SLOT(cmClearAll()))->setEnabled(phaseIdx == 0);

  contextMenu.exec(globalPos);
}

bool FlightModePanel::hasClipboardData(QByteArray * data) const
{
  const QClipboard * clipboard = QApplication::clipboard();
  const QMimeData * mimeData = clipboard->mimeData();
  if (mimeData->hasFormat(MIMETYPE_FLIGHTMODE)) {
    if (data)
      data->append(mimeData->data(MIMETYPE_FLIGHTMODE));
    return true;
  }
  return false;
}

bool FlightModePanel::insertAllowed() const
{
  return ((phaseIdx < fmCount - 1) && (model->flightModeData[fmCount - 1].isEmpty(fmCount - 1)));
}

bool FlightModePanel::moveDownAllowed() const
{
  return phaseIdx < fmCount - 1;
}

bool FlightModePanel::moveUpAllowed() const
{
  return phaseIdx > 0;
}

void FlightModePanel::cmClear()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear current Flight Mode properties. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;

  phase.clear(phaseIdx);

  if (phaseIdx == 0) {
    for (int i = 0; i < gvCount; i++) {
      model->gvarData[i].clear();
      for (int j = 0; j < fmCount; j++) {
        FlightModeData *fm = &model->flightModeData[j];
        fm->gvars[i] = fm->linkedGVarFlightModeZero(j);
      }
      model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_CLEAR, i);
    }
  }
  else {
    QComboBox * pswtch = ui->swtch;
    RawSwitch item = RawSwitch(SWITCH_TYPE_NONE);
    pswtch->setCurrentIndex(pswtch->findText(item.toString()));
    for (int i = 0; i < gvCount; i++) {
      phase.gvars[i] = phase.linkedGVarFlightModeZero(phaseIdx);
    }
    for (int i = 0; i < reCount; i++) {
      phase.rotaryEncoders[i] = phase.linkedEncoderFlightModeZero(phaseIdx);
    }
  }

  model->updateAllReferences(ModelData::REF_UPD_TYPE_FLIGHT_MODE, ModelData::REF_UPD_ACT_CLEAR, phaseIdx);

  update();
  emit modified();
  emit nameModified();
}

void FlightModePanel::cmClearAll()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear all Flight Modes. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;

  for (int j = 0; j < fmCount; j++) {
    FlightModeData *fm = &model->flightModeData[j];
    fm->clear(j);
    for (int i = 0; i < gvCount; i++) {
      fm->gvars[i] = fm->linkedGVarFlightModeZero(j);
    }
    for (int i = 0; i < reCount; i++) {
      fm->rotaryEncoders[i] = fm->linkedEncoderFlightModeZero(j);
    }
    model->updateAllReferences(ModelData::REF_UPD_TYPE_FLIGHT_MODE, ModelData::REF_UPD_ACT_CLEAR, j);
  }

  for (int i = 0; i < gvCount; i++) {
    model->gvarData[i].clear();
    model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_CLEAR, i);
  }

  emit datachanged();
  emit modified();
  emit nameModified();
}

void FlightModePanel::cmCopy()
{
  QByteArray data;
  data.append((char*)&model->flightModeData[phaseIdx], sizeof(FlightModeData));
  QMimeData *mimeData = new QMimeData;
  mimeData->setData(MIMETYPE_FLIGHTMODE, data);
  QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);
}

void FlightModePanel::cmCut()
{
  cmCopy();
  cmClear();
}

void FlightModePanel::cmDelete()
{
  if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Delete Flight Mode. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    return;

  int maxidx = fmCount - 1;

  for (int j = 0; j < maxidx; j++) {
    FlightModeData *fm = &model->flightModeData[j];
    if (j >= phaseIdx)
      memcpy(fm, &model->flightModeData[j + 1], sizeof(FlightModeData));

    for (int i = 0; i < trimCount; i++) {
      if (fm->trimRef[i] > phaseIdx && fm->trimMode[i] > -1)
        fm->trimRef[i]--;
      else if (fm->trimRef[i] == phaseIdx) {
        fm->trimMode[i] = 0;
        fm->trimRef[i] = 0;
        fm->trim[i] = 0;
      }
    }
    for (int i = 0; i < gvCount; i++) {
      if (model->isGVarLinked(j, i)) {
        int idx = model->getGVarFlightModeIndex(j, i);
        if (idx > phaseIdx)
          model->setGVarFlightModeIndexToValue(j, i, idx - 1);
        else if (idx == phaseIdx)
          model->setGVarFlightModeIndexToValue(j, i, 0);
      }
    }
    for (int i = 0; i < reCount; i++) {
      if (model->isEncoderLinked(j, i)) {
        int idx = model->getEncoderFlightModeIndex(j, i);
        if (idx > phaseIdx)
          model->setEncoderFlightModeIndexToValue(j, i, idx - 1);
        else if (idx == phaseIdx)
          model->setEncoderFlightModeIndexToValue(j, i, 0);
      }
    }
  }

  model->flightModeData[maxidx].clear(maxidx);
  model->updateAllReferences(ModelData::REF_UPD_TYPE_FLIGHT_MODE, ModelData::REF_UPD_ACT_SHIFT, phaseIdx, 0, -1);

  emit datachanged();
  emit modified();
  emit nameModified();
}

void FlightModePanel::cmInsert()
{
  int maxidx = fmCount - 1;

  for (int j = maxidx; j > 0; j--) {
    FlightModeData *fm = &model->flightModeData[j];
    if (j > phaseIdx)
      memcpy(fm, &model->flightModeData[j - 1], sizeof(FlightModeData));

    for (int i = 0; i < trimCount; i++) {
      if (phaseIdx == 0 and j == 1)
        fm->trimRef[i] = j;
      else if (fm->trimRef[i] >= phaseIdx) {
        if (fm->trimRef[i] < maxidx)
          fm->trimRef[i]++;
        else {
          fm->trimMode[i] = 0;
          fm->trimRef[i] = 0;
          fm->trim[i] = 0;
        }
      }
    }
    for (int i = 0; i < gvCount; i++) {
      if (model->isGVarLinked(j, i)) {
        int idx = model->getGVarFlightModeIndex(j, i);
        if (phaseIdx == 0 and j == 1)
          model->setGVarFlightModeIndexToValue(j, i, j);
        else if (idx >= phaseIdx)
          model->setGVarFlightModeIndexToValue(j, i, idx + 1);
      }
    }
    for (int i = 0; i < reCount; i++) {
      if (model->isEncoderLinked(j, i)) {
        int idx = model->getEncoderFlightModeIndex(j, i);
        if (phaseIdx == 0 and j == 1)
          model->setEncoderFlightModeIndexToValue(j, i, j);
        else if (idx >= phaseIdx)
          model->setEncoderFlightModeIndexToValue(j, i, idx + 1);
      }
    }
  }

  model->flightModeData[phaseIdx].clear(phaseIdx);
  model->updateAllReferences(ModelData::REF_UPD_TYPE_FLIGHT_MODE, ModelData::REF_UPD_ACT_SHIFT, phaseIdx, 0, 1);

  emit datachanged();
  emit modified();
  emit nameModified();
}

void FlightModePanel::cmMoveDown()
{
  swapData(phaseIdx, phaseIdx + 1);
}

void FlightModePanel::cmMoveUp()
{
  swapData(phaseIdx, phaseIdx - 1);
}

void FlightModePanel::cmPaste()
{
  QByteArray data;
  if (hasClipboardData(&data)) {
    memcpy(&model->flightModeData[phaseIdx], data.constData(), sizeof(FlightModeData));
    update();
    emit modified();
    emit nameModified();
  }
}

void FlightModePanel::swapData(int idx1, int idx2)
{
  if (idx1 != idx2) {
    FlightModeData fmdtmp = model->flightModeData[idx2];
    FlightModeData *fmd1 = &model->flightModeData[idx1];
    FlightModeData *fmd2 = &model->flightModeData[idx2];
    memcpy(fmd2, fmd1, sizeof(FlightModeData));
    memcpy(fmd1, &fmdtmp, sizeof(FlightModeData));
    model->updateAllReferences(ModelData::REF_UPD_TYPE_FLIGHT_MODE, ModelData::REF_UPD_ACT_SWAP, idx1, idx2);

    emit datachanged();
    emit modified();
    emit nameModified();
  }
}

void FlightModePanel::gvOnCustomContextMenuRequested(QPoint pos)
{
  QLabel *label = (QLabel *)sender();
  gvIdx = label->property("index").toInt();
  QPoint globalPos = label->mapToGlobal(pos);

  QMenu contextMenu;
  contextMenu.addAction(CompanionIcon("copy.png"), tr("Copy"), this, SLOT(gvCmCopy()));
  contextMenu.addAction(CompanionIcon("cut.png"), tr("Cut"), this, SLOT(gvCmCut()));
  contextMenu.addAction(CompanionIcon("paste.png"), tr("Paste"), this, SLOT(gvCmPaste()))->setEnabled(gvHasClipboardData());
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear"), this, SLOT(gvCmClear()));
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("arrow-right.png"), tr("Insert"), this, SLOT(gvCmInsert()))->setEnabled(gvInsertAllowed());
  contextMenu.addAction(CompanionIcon("arrow-left.png"), tr("Delete"), this, SLOT(gvCmDelete()))->setEnabled(gvDeleteAllowed());
  contextMenu.addAction(CompanionIcon("moveup.png"), tr("Move Up"), this, SLOT(gvCmMoveUp()))->setEnabled(gvMoveUpAllowed());
  contextMenu.addAction(CompanionIcon("movedown.png"), tr("Move Down"), this, SLOT(gvCmMoveDown()))->setEnabled(gvMoveDownAllowed());
  contextMenu.addSeparator();
  contextMenu.addAction(CompanionIcon("clear.png"), tr("Clear All"), this, SLOT(gvCmClearAll()));

  contextMenu.exec(globalPos);
}

bool FlightModePanel::gvHasClipboardData() const
{
  if (gvHasDefnClipboardData() || gvHasValueClipboardData())
    return true;
  return false;
}

bool FlightModePanel::gvHasDefnClipboardData(QByteArray * data) const
{
  const QClipboard * clipboard = QApplication::clipboard();
  const QMimeData * mimeData = clipboard->mimeData();
  if (phaseIdx == 0 && mimeData->hasFormat(MIMETYPE_GVAR_PARAMS)) {
    if (data) {
      data->clear();
      data->append(mimeData->data(MIMETYPE_GVAR_PARAMS));
    }
    return true;
  }
  return false;
}

bool FlightModePanel::gvHasValueClipboardData(QByteArray * data) const
{
  const QClipboard * clipboard = QApplication::clipboard();
  const QMimeData * mimeData = clipboard->mimeData();
  if (mimeData->hasFormat(MIMETYPE_GVAR_VALUE)) {
    if (data) {
      data->clear();
      data->append(mimeData->data(MIMETYPE_GVAR_VALUE));
    }
    return true;
  }
  return false;
}

bool FlightModePanel::gvDeleteAllowed() const
{
  return phaseIdx == 0;
}

bool FlightModePanel::gvInsertAllowed() const
{
  bool ret = true;
  if (phaseIdx == 0) {
    if (!model->gvarData[gvCount - 1].isEmpty())
      ret = false;
    for (int i = 0; i < fmCount; i++) {
      if (model->flightModeData[i].gvars[gvCount - 1] != phase.linkedGVarFlightModeZero(i))
        ret = false;
    }
  }
  else
    ret = false;

  return ret;
}

bool FlightModePanel::gvMoveDownAllowed() const
{
  return (phaseIdx == 0 && gvIdx < gvCount - 1);
}

bool FlightModePanel::gvMoveUpAllowed() const
{
  return (phaseIdx == 0 && gvIdx > 0);
}

void FlightModePanel::gvCmClear()
{
  if (phaseIdx == 0) {
    if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear selected Global Variable across all Flight Modes. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;
    model->gvarData[gvIdx].clear();
    for (int i = 0; i < fmCount; i++) {
      model->flightModeData[i].gvars[gvIdx] = model->flightModeData[i].linkedGVarFlightModeZero(i);
    }
    model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_CLEAR, gvIdx);
  }
  else {
    phase.gvars[gvIdx] = phase.linkedGVarFlightModeZero(phaseIdx);
  }
  emit datachanged();
  emit modified();
}

void FlightModePanel::gvCmClearAll()
{
  if (phaseIdx == 0) {
    if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear all Global Variables for all Flight Modes. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;
    for (int i = 0; i < gvCount; i++) {
      model->gvarData[i].clear();
      for (int j = 0; j < fmCount; j++) {
        model->flightModeData[j].gvars[i] = model->flightModeData[j].linkedGVarFlightModeZero(j);
      }
      model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_CLEAR, i);
    }
  }
  else {
    if (QMessageBox::question(this, CPN_STR_APP_NAME, tr("Clear all Global Variables for this Flight Mode. Are you sure?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
      return;
    for (int i = 0; i < gvCount; i++) {
      phase.gvars[i] = phase.linkedGVarFlightModeZero(phaseIdx);
    }
  }
  emit datachanged();
  emit modified();
}

void FlightModePanel::gvCmCopy()
{
  QMimeData *mimeData = new QMimeData;
  QByteArray data;
  if (phaseIdx == 0) {
    data.append((char*)&model->gvarData[gvIdx], sizeof(GVarData));
    mimeData->setData(MIMETYPE_GVAR_PARAMS, data);
  }
  else {
    mimeData->removeFormat(MIMETYPE_GVAR_PARAMS);
  }
  QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
  int val = phase.gvars[gvIdx];
  if (model->isGVarLinked(phaseIdx, gvIdx))
    val = GVAR_MAX_VALUE + 1 + model->getGVarFlightModeIndex(phaseIdx, gvIdx);  //  store index in case paste is to another flight mode
  data.setNum(val);
  mimeData->setData(MIMETYPE_GVAR_VALUE, data);
  QApplication::clipboard()->setMimeData(mimeData, QClipboard::Clipboard);
}

void FlightModePanel::gvCmCut()
{
  gvCmCopy();
  if (phaseIdx > 0) {
    gvCmClear();
  }
  else {
    model->gvarData[gvIdx].clear();
    phase.gvars[gvIdx] = 0;
    model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_CLEAR, gvIdx);
    emit datachanged();
    emit modified();
  }
}

void FlightModePanel::gvCmDelete()
{
  if (phaseIdx > 0)
    return;
  int maxidx = gvCount - 1;
  for (int i = gvIdx; i < maxidx; i++) {
    memcpy(&model->gvarData[i], &model->gvarData[i + 1], sizeof(GVarData));
    for (int j = 0; j < fmCount; j++) {
      model->flightModeData[j].gvars[i] = model->flightModeData[j].gvars[i + 1];
    }
  }

  model->gvarData[maxidx].clear();
  for (int j = 0; j < fmCount; j++) {
    model->flightModeData[j].gvars[maxidx] = model->flightModeData[j].linkedGVarFlightModeZero(j);
  }
  model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_SHIFT, gvIdx, 0, -1);
  emit datachanged();
  emit modified();
}

void FlightModePanel::gvCmInsert()
{
  for (int i = (gvCount - 1); i > gvIdx; i--) {
    memcpy(&model->gvarData[i], &model->gvarData[i - 1], sizeof(GVarData));
    for (int j = 0; j < fmCount; j++) {
      model->flightModeData[j].gvars[i] = model->flightModeData[j].gvars[i - 1];
    }
  }
  model->gvarData[gvIdx].clear();
  for (int j = 0; j < fmCount; j++) {
    model->flightModeData[j].gvars[gvIdx] = model->flightModeData[j].linkedGVarFlightModeZero(j);
  }
  model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_SHIFT, gvIdx, 0, 1);
  emit datachanged();
  emit modified();
}

void FlightModePanel::gvCmMoveDown()
{
  if (phaseIdx == 0)
    gvSwapData(gvIdx, gvIdx + 1);
}

void FlightModePanel::gvCmMoveUp()
{
  if (phaseIdx == 0)
    gvSwapData(gvIdx, gvIdx - 1);
}

void FlightModePanel::gvCmPaste()
{
  QByteArray data;
  if (phaseIdx == 0 && gvHasDefnClipboardData(&data)) {
    GVarData *gvd = &model->gvarData[gvIdx];
    memcpy(gvd, data.constData(), sizeof(GVarData));
  }
  if (gvHasValueClipboardData(&data)) {
    int val = data.toInt();
    if (val > GVAR_MAX_VALUE) {
      if (phaseIdx > 0)
        model->setGVarFlightModeIndexToValue(phaseIdx, gvIdx, val - (GVAR_MAX_VALUE + 1));    //  index stored in clipboard
      else
        phase.gvars[gvIdx] = 0;
    }
    else
      phase.gvars[gvIdx] = val;
  }
  emit datachanged();
  emit modified();
}

void FlightModePanel::gvSwapData(int idx1, int idx2)
{
  if (phaseIdx == 0 && idx1 != idx2) {
    GVarData datatmp = model->gvarData[idx2];
    GVarData *data1 = &model->gvarData[idx1];
    GVarData *data2 = &model->gvarData[idx2];
    memcpy(data2, data1, sizeof(GVarData));
    memcpy(data1, &datatmp, sizeof(GVarData));
    for (int j = 0; j < fmCount; j++) {
      int valtmp = model->flightModeData[j].gvars[idx2];
      model->flightModeData[j].gvars[idx2] = model->flightModeData[j].gvars[idx1];
      model->flightModeData[j].gvars[idx1] = valtmp;
    }
    model->updateAllReferences(ModelData::REF_UPD_TYPE_GLOBAL_VARIABLE, ModelData::REF_UPD_ACT_SWAP, idx1, idx2);
    emit datachanged();
    emit modified();
  }
}

/**********************************************************/

FlightModesPanel::FlightModesPanel(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware):
  ModelPanel(parent, model, generalSettings, firmware),
  modesCount(firmware->getCapability(FlightModes))
{

  RawSwitchFilterItemModel * swModel = new RawSwitchFilterItemModel(&generalSettings, &model, RawSwitch::MixesContext, this);
  connect(this, &FlightModesPanel::updated, swModel, &RawSwitchFilterItemModel::update);

  QGridLayout * gridLayout = new QGridLayout(this);
  tabWidget = new QTabWidget(this);
  for (int i = 0; i < modesCount; i++) {
    FlightModePanel * tab = new FlightModePanel(tabWidget, model, i, generalSettings, firmware, swModel);
    tab->setProperty("index", i);
    connect(tab,  &FlightModePanel::datachanged,  this, &FlightModesPanel::update);
    connect(tab,  &FlightModePanel::modified,     this, &FlightModesPanel::modified);
    connect(tab,  &FlightModePanel::nameModified, this, &FlightModesPanel::onPhaseNameChanged);
    connect(this, &FlightModesPanel::updated,     tab,  &FlightModePanel::update);
    tabWidget->addTab(tab, getTabName(i));
  }
  gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
}

FlightModesPanel::~FlightModesPanel()
{
}

QString FlightModesPanel::getTabName(int index)
{
  QString result = tr("Flight Mode %1").arg(index);
  const char *name = model->flightModeData[index].name;
  if (firmware->getCapability(FlightModesName) && strlen(name) > 0) {
    result += tr(" (%1)").arg(name);
  }
  else if (index == 0) {
    result += tr(" (default)");
  }

  return result;
}

void FlightModesPanel::onPhaseNameChanged()
{
  int index = sender()->property("index").toInt();
  tabWidget->setTabText(index, getTabName(index));
}

void FlightModesPanel::update()
{
  emit updated();
}
