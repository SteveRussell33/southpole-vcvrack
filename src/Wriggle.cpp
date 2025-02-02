#include <iostream>

#include "Southpole.hpp"

struct Wriggle : Module {
  enum ParamIds {
    DAMP_PARAM,
    TENS_PARAM,
    SCALE_PARAM,
    OFFSET_PARAM,
    NUM_PARAMS
  };
  enum InputIds {
    IN_INPUT,
    DAMP_INPUT,
    TENS_INPUT,
    SCALE_INPUT,
    OFFSET_INPUT,
    NUM_INPUTS
  };
  enum OutputIds {
    OUT_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds {
    //		DECAY1_LIGHT,
    NUM_LIGHTS
  };

  float a0 = 0.0;
  float v0 = 0.0;
  float x0 = 0.0;

  Wriggle() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

    configParam(Wriggle::TENS_PARAM, 0.0, 1.0, 0.5, "");
    configParam(Wriggle::DAMP_PARAM, 0.0, 1.0, 0.5, "");
    configParam(Wriggle::SCALE_PARAM, 0.0, 1.0, 0.5, "");
    configParam(Wriggle::OFFSET_PARAM, 0.0, 1.0, 0.5, "");
  }

  void process(const ProcessArgs &args) override;
};

void Wriggle::process(const ProcessArgs &args) {

  float dt = 1. / args.sampleRate;

  float tens = clamp(params[TENS_PARAM].getValue() + inputs[TENS_INPUT].getNormalVoltage(0.) / 10.0, 0.0f, 1.0f);
  float damp = clamp(params[DAMP_PARAM].getValue() + inputs[DAMP_INPUT].getNormalVoltage(0.) / 10.0, 0.0f, 1.0f);
  float scale = clamp(params[SCALE_PARAM].getValue() + inputs[SCALE_INPUT].getNormalVoltage(0.) / 10.0, 0.0f, 1.0f);
  float offset = clamp(params[OFFSET_PARAM].getValue() + inputs[OFFSET_INPUT].getNormalVoltage(0.) / 10.0, 0.0f, 1.0f);

  // semi-parametric Euler... (i know ...)

  //float m  = 1000.*pow(2., 6.*mass);
  float k = pow(2., 18. * tens);
  float b = pow(2., 10. * damp - 3.);
  float xin = inputs[IN_INPUT].getVoltage();

  v0 = v0 + dt * a0; // - v0 / m;
  x0 = x0 + dt * v0;
  a0 = -k * (x0 - xin) - b * v0; //) / m;

  outputs[OUT_OUTPUT].setVoltage(clamp((10. * offset - 5.) + scale * x0, -10.0f, 10.0f));
}

struct WriggleWidget : ModuleWidget {

  WriggleWidget(Wriggle *module) {
    setModule(module);

    box.size = Vec(15 * 2, 380);

    {
      SvgPanel *panel = new SvgPanel();
      panel->box.size = box.size;
      panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Wriggle.svg")));
      addChild(panel);
    }

    const float x1 = 5.;
    const float y1 = 40.;
    const float yh = 31.;

    addInput(createInput<sp_Port>(Vec(x1, y1 + 0 * yh), module, Wriggle::IN_INPUT));
    addInput(createInput<sp_Port>(Vec(x1, y1 + 1.125 * yh), module, Wriggle::TENS_INPUT));
    addParam(createParam<sp_SmallBlackKnob>(Vec(x1, y1 + 2 * yh), module, Wriggle::TENS_PARAM));
    addInput(createInput<sp_Port>(Vec(x1, y1 + 3.125 * yh), module, Wriggle::DAMP_INPUT));
    addParam(createParam<sp_SmallBlackKnob>(Vec(x1, y1 + 4 * yh), module, Wriggle::DAMP_PARAM));
    addInput(createInput<sp_Port>(Vec(x1, y1 + 5.125 * yh), module, Wriggle::SCALE_INPUT));
    addParam(createParam<sp_SmallBlackKnob>(Vec(x1, y1 + 6 * yh), module, Wriggle::SCALE_PARAM));
    addInput(createInput<sp_Port>(Vec(x1, y1 + 7.125 * yh), module, Wriggle::OFFSET_INPUT));
    addParam(createParam<sp_SmallBlackKnob>(Vec(x1, y1 + 8 * yh), module, Wriggle::OFFSET_PARAM));
    addOutput(createOutput<sp_Port>(Vec(x1, y1 + 9.25 * yh), module, Wriggle::OUT_OUTPUT));

    //	addChild(createLight<SmallLight<RedLight>>(Vec(94, 109), module, Wriggle::DECAY_LIGHT));
  }
};

Model *modelWriggle = createModel<Wriggle, WriggleWidget>("Wriggle");
