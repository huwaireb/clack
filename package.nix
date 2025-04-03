{
  lib,
  stdenv,
  llvm,
  imgui,
  glfw,
  ...
}:
let
  fs = lib.fileset;
in
stdenv.mkDerivation {
  pname = "calculator";
  version = "0.1.0";
  src = fs.toSource {
    root = ./.;
    fileset = fs.unions [ ./src ];
  };

  outputs = [
    "out"
    "development"
  ];

  buildInputs = [
    llvm.libcxx
    (imgui.override { IMGUI_BUILD_GLFW_BINDING = true; })
    glfw
  ];

  FLAGS = [
    "--start-no-unused-arguments"
    "-std=c++23"
    "-stdlib=libc++"
    "-fstrict-enums"
    "-fsanitize=undefined"
    "-fsanitize=address"
    "-flto"
    "-fno-operator-names"
    "-fno-common"
    "-fvisibility=hidden"
    "-Wall"
    "-Wconversion"
  ];

  buildPhase = ''
    $CXX src/main.cc -limgui -lglfw \
                     ${lib.optionalString stdenv.isDarwin "-framework OpenGL"} \
                     -o calculator -MJ calculator.o.json $FLAGS
  '';

  installPhase = ''
    install -D -t $out/bin calculator
    install -D -t $development/fragments *.o.json
  '';

  meta.mainProgram = "calculator";
}
