{
  lib,
  stdenv,
  llvm,
  imgui,
  glfw,
  ...
}:
stdenv.mkDerivation {
  pname = "clack";
  version = "0.1.0";
  src = lib.fileset.toSource {
    root = ./.;
    fileset = lib.fileset.unions [ ./src ];
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
                     -o clack -MJ clack.o.json $FLAGS
  '';

  installPhase = ''
    install -D -t $out/bin clack
    install -D -t $development/fragments *.o.json
  '';

  meta.mainProgram = "clack";
}
