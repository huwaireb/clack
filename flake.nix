{
  description = "huwaireb/dsa.calculator: Mini Project 1 for EECE350";

  outputs =
    inputs@{ parts, ... }:
    parts.lib.mkFlake { inherit inputs; } {
      perSystem =
        {
          lib,
          pkgs,
          config,
          ...
        }:
        let
          llvm = pkgs.llvmPackages_19;
          stdenv = llvm.stdenv;
        in
        {
          packages.calculator = pkgs.callPackage ./package.nix { inherit stdenv llvm; };

          devShells.default = pkgs.mkShell.override { inherit stdenv; } {
            packages = [ (pkgs.ccls.override { llvmPackages = llvm; }) ];
            inputsFrom = [ config.packages.default ];
          };

          apps.ccdb = {
            type = "app";
            program = lib.getExe (
              pkgs.writeShellApplication {
                name = "ccdb";
                runtimeInputs = [ pkgs.gnused ];
                text =
                  let
                    proj = config.packages.calculator.development;
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

          packages.default = config.packages.calculator;
        };
      systems = [ "aarch64-darwin" ];
    };

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };
}
