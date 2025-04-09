{
  description = ''
    huwaireb/clack: A calculator written in C++23 with a GUI using imgui. Built with Nix (SP)
  '';

  outputs =
    inputs@{ parts, ... }:
    parts.lib.mkFlake { inherit inputs; } {
      systems = [ "aarch64-darwin" ];
      perSystem =
        { pkgs, config, ... }:
        let
          llvm = pkgs.llvmPackages_19;
          stdenv = llvm.stdenv;
        in
        {
          packages.clack = pkgs.callPackage ./package.nix { inherit stdenv llvm; };

          devShells.default = pkgs.mkShell.override { inherit stdenv; } {
            packages = [ (pkgs.ccls.override { llvmPackages = llvm; }) ];
            inputsFrom = [ config.packages.default ];
          };

          apps.ccdb = {
            type = "app";
            program = pkgs.lib.getExe (
              pkgs.writeShellApplication {
                name = "ccdb";
                runtimeInputs = [ pkgs.gnused ];
                text =
                  let
                    proj = config.packages.clack.development;
                  in
                  ''
                    sed  -e '1s/^/[\n/' -e '$s/,$/\n]/' \
                         -e "s|/private/tmp/nix-build-${proj.name}.drv-0\(/source\)\?|$(pwd)|g" \
                         ${proj}/fragments/*.o.json \
                         > compile_commands.json
                  '';
              }
            );
          };

          packages.default = config.packages.clack;
        };
    };

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };
}
