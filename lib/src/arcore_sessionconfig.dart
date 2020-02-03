class ARCoreSessionConfig {
  final PlaneFindingMode planeFindingMode;

  const ARCoreSessionConfig({
    this.planeFindingMode,
  });

  Map<String, dynamic> toMap() => <String, dynamic>{
        'planeFindingMode': planeFindingMode._value,
      };
}

class PlaneFindingMode {
  static const PlaneFindingMode DISABLED = PlaneFindingMode._(0, 'DISABLED');
  static const PlaneFindingMode HORIZONTAL =
      PlaneFindingMode._(1, 'HORIZONTAL');
  static const PlaneFindingMode VERTICAL = PlaneFindingMode._(2, 'VERTICAL');
  static const PlaneFindingMode HORIZONTAL_AND_VERTICAL =
      PlaneFindingMode._(3, 'HORIZONTAL_AND_VERTICAL');
  static final values = [
    DISABLED,
    HORIZONTAL,
    VERTICAL,
    HORIZONTAL_AND_VERTICAL,
  ];

  const PlaneFindingMode._(this._value, this._text);

  final int _value;
  final String _text;

  static PlaneFindingMode get(int value) {
    PlaneFindingMode ret = DISABLED;
    values.forEach((m) {
      if (m._value == value) {
        ret = m;
        return;
      }
    });
    return ret;
  }

  @override
  String toString() {
    return '$_text - $_value';
  }
}
